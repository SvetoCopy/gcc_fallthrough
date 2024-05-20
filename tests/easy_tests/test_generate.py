import os

template = '''// -ftrivial-auto-var-init=zero -O1
// -ftrivial-auto-var-init=zero -O2
// -ftrivial-auto-var-init=zero -O3
// -ftrivial-auto-var-init=zero -O0
// -ftrivial-auto-var-init=pattern

#include "stdio.h"

<type> test{num}_main(int b) {{
    if (b == 3) {{
        return {ret_val_1};
    }}
}} 

<type> test{num}_help() {{
    return {ret_val_2};
}}

int main() {{
    <type> res = test{num}_main(3);
    if (res == {ret_val_2}) printf("TEST_{num} FAILED");
    else          printf("TEST_{num} PASSED");
}} 
'''

data = [
    {'type': 'int', 'num': 1, 'ret_val_1': 2, 'ret_val_2': 1},
    {'type': 'float', 'num': 2, 'ret_val_1': 2, 'ret_val_2': 1},
    {'type': 'double', 'num': 3, 'ret_val_1': 2, 'ret_val_2': 1},
    {'type': 'char', 'num': 4, 'ret_val_1': 2, 'ret_val_2': 1}
]

for d in data:
    file_name = f"t{d['num']}.cpp"
    with open(file_name, 'w') as f:
        content = template.format(num=d['num'], ret_val_1=d['ret_val_1'], ret_val_2=d['ret_val_2'])
        content = content.replace('<type>', d['type'])
        f.write(content)

print("Файлы успешно созданы.")