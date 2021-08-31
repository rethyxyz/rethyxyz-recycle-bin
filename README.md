# rb
### Summary
A Windows like "recycle bin", and safer alternative to rm, for Unix-like systems. Written in POSIX compliant Shell code.

### Basic Usage
`rb.sh [FILE] [FILE 2] [FILE 3] ...`

List the file(s) or directory(s) you want to remove. You may want to create an alias in place of rm that points to `rb.sh`.

### FAQs
#### How does it work?
It works the same as rm, but doesn't take any flags. After pseudo-removal, the file or directory is placed into the ~/.Trash directory.
