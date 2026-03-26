# CPGen

## Architectural choices

### CLI

Cli is old fashionned, maybe a command register would've been better, looking fwd to it when I'm not too lazy as I'll have to think a bit.

### TUI

I went for callback based ui, it's cleaner and separates each responsibility clearly, child knows nothing about parent whilst preserving hierarchy.

Need to switch to [utf8proc](https://github.com/JuliaStrings/utf8proc) so that in /TUI/Misc/Utils.hpp (visualWidth) can be cross platform
Only works on posix rn.

## Tests

Unit test other code parts.

### FS Writer

Dumb to UT FS writer as it relies on std but coverage.
