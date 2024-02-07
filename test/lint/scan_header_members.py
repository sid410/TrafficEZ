import re
import sys
import os

def check_public_before_private(filename):
    with open(filename, 'r') as file:
        content = file.read()
    
    classes = re.findall(r'class\s+\w+\s*{([^}]+)};', content, re.DOTALL)
    
    for class_content in classes:
        public_pos = class_content.find('public:')
        private_pos = class_content.find('private:')
        
        if public_pos == -1 or private_pos == -1:
            continue  # Skip if either is not found
        
        if private_pos < public_pos:
            print(f"Check failed in {filename}")
            return False
    
    return True

def scan_directory_for_headers(directory):
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.h'):
                filepath = os.path.join(root, file)
                if not check_public_before_private(filepath):
                    return False
    return True

# Warning: this check will return success
# if it did not find any header files to check.
if __name__ == "__main__":
    print("checking class members order...")

    directory = sys.argv[1] if len(sys.argv) > 1 else '.'
    if not scan_directory_for_headers(directory):
        sys.exit(1)
    
    print("checking successful!")
    sys.exit(0)

