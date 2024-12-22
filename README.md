# Small Tools Collection

## Colorize CLI Tool

`colorize` is a command-line tool for applying ANSI color highlights to text patterns in a stream of text.

### Supported Colors

Foreground and background colors:

- Standard: `black`, `red`, `green`, `yellow`, `blue`, `magenta`, `cyan`, `white`
- Bright: `bright_black`, `bright_red`, `bright_green`, `bright_yellow`, `bright_blue`, `bright_magenta`, `bright_cyan`, `bright_white`
- Additional: `dark_gray`, `bright_gray`

Colors can be combined with backgrounds using a colon (`:`). For example, `bright_red:dark_gray` applies a bright red foreground on a dark gray background.

### Usage

```
colorize -m <string1> <color1> [-m <string2> <color2> ...] [-b <start> <end> <color> ...] [--help]
```

#### Options

- `-m <string> <color>`: Matches and highlights occurrences of `<string>` with `<color>`.
- `-b <start> <end> <color>`: Highlights content between `<start>` and `<end>` with `<color>`.
- `--help`: Displays usage information and exits.



