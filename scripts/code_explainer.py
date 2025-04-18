import os
import json
import hashlib
from pathlib import Path
from groq import Groq
import argparse
from dotenv import load_dotenv

# Load environment variables from .env file
load_dotenv()

def get_file_hash(file_path):
    """Generate a hash of the file content to track changes."""
    with open(file_path, 'rb') as f:
        return hashlib.md5(f.read()).hexdigest()

def get_language_prompt(file_extension):
    """Return appropriate prompt based on file extension."""
    language_prompts = {
        '.java': "Java",
        '.c': "C",
        '.cpp': "C++",
        '.h': "C/C++ header",
        '.py': "Python",
        '.js': "JavaScript",
        '.rs': "Rust",
        '.rb': "Ruby",
        '.cs': "C#",
    }
    
    return language_prompts.get(file_extension.lower(), "code")

def explain_code(client, file_path, file_content):
    """Send code to Groq API for explanation."""
    file_extension = os.path.splitext(file_path)[1]
    language = get_language_prompt(file_extension)
    
    try:
        completion = client.chat.completions.create(
            model="llama-3.3-70b-versatile",
            messages=[
                {"role": "system", "content": f"You are an expert programmer and computer science faculty member. Explain the following {language} code in detail, covering its purpose, functionality, techniques used. For each section of the code give explanation and respective line number and Focus on explaining what it does and how it works. Structure your explanation with clear sections and be thorough in your analysis."},
                {"role": "user", "content": f"Explain this {language} code:\n\n```{language.lower()}\n{file_content}\n```"}
            ],
            temperature=0.6,
            max_tokens=4096,  
            top_p=0.95,
            stream=False,
            stop=None,
        )
        return completion.choices[0].message.content
    except Exception as e:
        print(f"Error explaining {file_path}: {e}")
        return f"Error: {str(e)}"

def process_code_files(root_dir, output_file, api_key, file_extensions):
    """Process all code files with specified extensions in the directory and its subdirectories."""
    # Initialize Groq client
    client = Groq(api_key=api_key)
    
    # Load existing explanations if the output file exists
    explanations = {}
    file_hashes = {}
    
    if os.path.exists(output_file):
        try:
            with open(output_file, 'r') as f:
                data = json.load(f)
                explanations = data.get('explanations', {})
                file_hashes = data.get('file_hashes', {})
        except json.JSONDecodeError:
            print(f"Warning: Could not parse {output_file}. Starting with empty explanations.")
    
    # Find all code files with specified extensions
    code_files = []
    for ext in file_extensions:
        code_files.extend(list(Path(root_dir).rglob(f"*{ext}")))
    
    total_files = len(code_files)
    print(f"Found {total_files} code files to process with extensions: {', '.join(file_extensions)}")
    
    # Process each file
    for i, file_path in enumerate(code_files):
        file_path_str = str(file_path)
        rel_path = os.path.relpath(file_path_str, root_dir)
        
        # Calculate file hash
        current_hash = get_file_hash(file_path_str)
        
        # Skip if file hasn't changed
        if rel_path in file_hashes and file_hashes[rel_path] == current_hash and rel_path in explanations:
            print(f"[{i+1}/{total_files}] Skipping unchanged file: {rel_path}")
            continue
        
        print(f"[{i+1}/{total_files}] Processing: {rel_path}")
        
        # Read file content
        try:
            with open(file_path_str, 'r', encoding='utf-8') as f:
                try:
                    file_content = f.read()
                except UnicodeDecodeError:
                    # Try with a different encoding
                    with open(file_path_str, 'r', encoding='latin-1') as f2:
                        file_content = f2.read()
        except Exception as e:
            print(f"Error: Could not read {file_path_str}. Skipping. Error: {e}")
            continue
        
        # Get explanation from Groq
        explanation = explain_code(client, file_path_str, file_content)
        
        # Store results
        explanations[rel_path] = {
            "explanation": explanation,
            "language": get_language_prompt(os.path.splitext(file_path_str)[1]),
            "file_name": os.path.basename(file_path_str),
            "last_updated": os.path.getmtime(file_path_str)
        }
        file_hashes[rel_path] = current_hash
        
        # Save after each file to prevent data loss
        with open(output_file, 'w') as f:
            json.dump({
                'explanations': explanations,
                'file_hashes': file_hashes
            }, f, indent=2)
        
        print(f"Saved explanation for {rel_path}")
    
    print(f"Completed processing {total_files} files. Results saved to {output_file}")
    
    # Debug: Print the current directory and check if the output file exists
    print(f"Output file exists: {os.path.exists(output_file)}")
    print(f"Output file size: {os.path.getsize(output_file) if os.path.exists(output_file) else 'N/A'}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Explain code using Groq API")
    
    parser.add_argument("--root", default="f:\\OS Code", help="Root directory to scan for code files")
    parser.add_argument("--output", default="f:\\OS Code\\explanations.json", help="Output JSON file")
    parser.add_argument("--api-key", help="Groq API key (can also be set via GROQ_API env variable)")
    parser.add_argument("--extensions", default=".java,.c,.cpp,.h,.py", help="Comma-separated list of file extensions to process")
    
    args = parser.parse_args()
    
    # Get API key from args or environment variable
    api_key = args.api_key or os.getenv("GROQ_API") or os.getenv("GROQ_API_KEY")
    if not api_key:
        raise ValueError("API key must be provided either via --api-key argument or GROQ_API/GROQ_API_KEY environment variable")
    
    # Parse file extensions
    file_extensions = args.extensions.split(",")
    
    # Print debug info
    print(f"Root directory: {args.root}")
    print(f"Output file: {args.output}")
    print(f"File extensions: {file_extensions}")
    
    process_code_files(args.root, args.output, api_key, file_extensions)