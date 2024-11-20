import re

# Function to modify comments from '//' to '/* */\' format, preserving indentation
def modify_comments_in_file_with_backslash(file_path: str):
    # Read the content from the given file
    with open(file_path, 'r') as file:
        content = file.readlines()

    # Modify the comments using regular expressions, preserving original indentation
    modified_content = []
    for index, line in enumerate(content):
        # Modify all lines except the last one to add a backslash
        if index < len(content) - 1:
            modified_line = re.sub(r'( *)(//)(.*)', r'\1/*\3 */\\', line)
        else:
            # Do not add backslash to the last line
            modified_line = re.sub(r'( *)(//)(.*)', r'\1/*\3 */', line)
        modified_content.append(modified_line)

    # Save the modified content back to the file
    with open(file_path, 'w') as file:
        file.writelines(modified_content)

# Example usage
file_path = 'Descriptor.h'
modify_comments_in_file_with_backslash(file_path)

