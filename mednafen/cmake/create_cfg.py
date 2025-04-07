import re

with open('./config.h.in', 'r') as file:
    content = file.read()

new_content = re.sub(r'^(#undef\s+(\w+))$', r'#cmakedefine \2 @\2@', content, flags=re.MULTILINE)

with open('cmake.config.h.in', 'w') as file:
    file.write(new_content)