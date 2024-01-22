## VS Code extension: Syntax Highlighting definition for the Elan programming language 

uho 2024-01-21

### Installation

From the VS Code *Command Palette* (search for *VSIX*,) use the command `Extension: Install from VSIX` and point it to `elan-x.y.z.vsix`

### Repackage the .vsix extension file

```bash
> cd .vscode/extensions/elan
> vsce package
```

This creates `elan-x.y.z.vsix`

(Get `vsce` with `npm install -g @vscode/vsce`)

### Customize

You can add entries to the user's `settings.json` to customize the colors. For example:

```json
{
    "editor.tokenColorCustomizations": {
        "textMateRules": [
            {
                "scope": "keyword.operator.elan",
                "settings": {
                    "foreground": "#9090FF"
                }
            }
        ]
    },
    "workbench.colorTheme": "Monokai"
}
```
