# LiteScript for Visual Studio Code

Support of the [LiteScript](https://github.com/LePtitDev/lite-script) language (`.lsc` files) and of its assembly language (`.lsa` files).

The language server and the debugger are given by the playground program `litescript-pg` (`litescript-pg --lsp` and `litescript-pg --dap`): it must be installed in the `PATH`, or its path must be given by the setting `litescript.playgroundPath`.

## Features

- **Syntax highlighting** of the scripts, of the assembly code and of the ` ```lsc ` blocks of the Markdown files
- **Diagnostics** while typing: syntax errors (given by the compiler of the engine) and names that aren't declared
- **Completion**: variables visible from the position, members of the classes (`this.`), namespaces, libraries of the playground (`Math.`, `File.`…), methods of the arrays and the strings, keywords and snippets
- **Hover** and **signature help** for the declarations of the file and the libraries
- **Go to definition**, **find references**, **rename**, **highlight** of the occurrences
- **Outline** (classes, members, functions, namespaces) and **folding**
- **Debugging**: breakpoints (with conditions and log messages), steps, call stack, variables (modifiable), watch expressions and debug console, stop on the uncaught errors
- **Commands**: *LiteScript: Run File* (in a terminal), *LiteScript: Debug File*, *LiteScript: Show Assembly Code*, *LiteScript: Restart Language Server*
- **Tasks** and problem matcher: the errors of `litescript-pg` appear in the Problems panel

## Settings

| Setting | Description |
|---------|-------------|
| `litescript.playgroundPath` | Path of `litescript-pg` (empty: searched in the `PATH`) |
| `litescript.diagnostics.unknownVariables` | Report the names that aren't declared (disable it for scripts that use functions of another application) |
| `litescript.trace.server` | Trace the messages of the language server |

## Debugging

F5 on a `.lsc` file debugs it without configuration. A configuration in `.vscode/launch.json`:

```json
{
    "type": "litescript",
    "request": "launch",
    "name": "Debug the current file",
    "program": "${file}",
    "args": ["first", "second"],
    "cwd": "${workspaceFolder}",
    "stopOnEntry": false
}
```

The output of the script goes to the debug console. The standard input isn't available while debugging (`Console.readLine()` gives `null`).

The debug console evaluates expressions and instructions in the current function (`var x = 2;`, `values.push(4)`). Hovers and the outer frames of the call stack only read variables (`a.b[0]`).

## Tasks

```json
{
    "label": "Tests",
    "type": "litescript",
    "files": ["tests/**/*.lsc"],
    "options": ["--bail"],
    "problemMatcher": ["$litescript"]
}
```

## Building the extension

```bash
npm install
npx vsce package
code --install-extension litescript-0.1.0.vsix
```
