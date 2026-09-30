# VS Code 

## Extensions (recommended)
- [Clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) for IntelliSense. Also integrates with **clang-format**, **clang-tidy**, and `compile_commands.json`. Recommended version 22 or superior for doxygen support.
- [Microsoft C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) for GDB debugging. Disable its IntelliSense to avoid conflicts with Clangd.
- [Latex workshop](https://marketplace.visualstudio.com/items?itemName=James-Yu.latex-workshop)
- [Live share](https://marketplace.visualstudio.com/items?itemName=MS-vsliveshare.vsliveshare)
- [Todo tree](https://marketplace.visualstudio.com/items?itemName=Gruntfuggly.todo-tree)
- [Github Actions](https://marketplace.visualstudio.com/items?itemName=GitHub.vscode-github-actions)


## VS Code Settings (Recommended)

Open with `Ctrl+Shift+P` → **Open User Settings (JSON)**, then add:

```json
{
	"C_Cpp.intelliSenseEngine": "disabled",
    "[c]": {
        "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd",
        "editor.formatOnSave": true,
        "editor.formatOnType": true,
        "editor.autoIndentOnPaste": false,
        "editor.tabSize": 8,
        "editor.detectIndentation": false
    },
    "editor.inlayHints.enabled": "off",
    "makefile.configureOnOpen": false
}
```

## VS Code debugger setup
After installing [Microsoft C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) extension, on `.vscode` folder at the top directory add these 2 files:

### launch.json
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug program",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/program",
            "args": [
                "-c on",
                "-f test"
            ],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "gdb",
            "preLaunchTask": "build-debug"
        }
    ]
}
```

### task.json
```json
// Compile with gcc for better integration with gdb debuger.
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build-debug",
            "type": "shell",
            "command": "make",
            "args": [
                "rebuild",
                "MODE=debug",
                "CC=gcc",
                "TARGET=./build/program"
            ],
            "options": {
                "cwd": "${workspaceFolder}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}
```
