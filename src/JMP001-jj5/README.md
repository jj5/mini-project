# JMP001: Symbol Keyboard

This project is from the
[May 2024 issue](https://www.siliconchip.com.au/Issue/2024/May)
of
[Silicon Chip magazine](https://www.siliconchip.com.au/).

There is more info about these projects on the web over here:

- https://www.jj5.net/wiki/Mini_Projects

The magazine article that introduced this project is on the web over here:

- https://www.siliconchip.com.au/Issue/2024/May/Symbol+USB+Keyboard

The hardware for this project is the Arduino Leonardo and the XC4630 touch screen:

- https://www.jaycar.com.au/duinotech-leonardo-r3-main-board/p/XC4430
- https://www.jaycar.com.au/duinotech-arduino-compatible-25-inch-colour-lcd-touch-screen-display/p/XC4630

The base directory for this project is the directory which this `README.md` file is in. All referenced files and directories
are given relative to this base directory.

The Arduino project for this code is in `ino/Symbol_Keyboard/Symbol_Keyboard.ino`. You can open this project in your
Arduino IDE.

You might need to add the `Keyboard.h` library which is provided by Arduino. You can add this library using the library
management facilities in the Arduino IDE. Search for "Arduino Keyboard" and ye shall find.

The XC4430 library is included in the `ino/Symbol_Keyboard/XC4630d.c` file which is provided with this code.

There are scripts in the `bin` directory for doing various code-generation tasks. The main task is to generate C header
files which include the bitmap font data for each symbol. To run all the code generation there is a main script:
`bin/gen.sh`. Just run `bin/gen.sh` and all the code-generation subtasks will be done. If you're interested in the
details just read `bin/gen.sh` and find the subcommands specified in the `bin/libexec/` directory. Altogether these
code-generation scripts run in BASH and PHP, so you will need those installed, along with standard tools such as `sed` and
`awk` and `grep` etc.

The specification for each button we might like to support is in a simple machine-readable file: `doc/spec.txt`. When
updating the `doc/spec.txt` file it is advisable to type in the four-digit alt-key code, and then actually use that alt-key
code to insert the appropriate symbol. This is to avoid any problems that might arise from having the wrong symbols
associated with the wrong bitmap files. When it comes time to actually edit the symbol bitmap file it is also advisable
to copy and paste the symbol into your graphics program from the specification file. Again, this is to minimize the
possibility of error, but it is also convenient.

The `doc/spec-sorted.txt` file is automatically generated from the `doc/spec.txt` file. It is, as the name suggests, a
sorted version of the `doc/spec.txt` specification file. This was useful when creating the initial batch of bitmaps as those
were sorted by file name in Windows Explorer as I edited each of them in Microsoft Paint.

The `bin/gen.sh` script runs all the necessary code-generation tasks for the project. If you change the `doc/spec.txt` or
any of the bitmaps in the `bmp` directory you should re-run the `bin/gen.sh` script to pickup any changes.

Be aware that the `bin/gen.sh` script will overwrite any files that it is responsible for maintaining. So you don't want to
make manual changes to such files because those changes will be overwritten the next time code-generation is run.

The `bin/gen.sh` script processes the `doc/spec.txt` specification to generate bitmaps for each symbol from the template
bitmap which is in `etc/template.bmp`. The template.bmp file is a 64 pixel by 64 pixel square monochrome bitmap with a one
pixel border. If a symbol bitmap file doesn't already exist the template bitmap is copied to a new file in the `bmp`
directory for each supported symbol as defined in `doc/spec.txt`.

After a bitmap file is created from a template you need to edit it in a graphics editor (such as Microsoft Paint) with an
appropriate symbol with an appropriate font. The font I have used for the symbols I generated is 48-point Calibri. Use a
black background and a white foreground.

The symbol font bitmaps are in the `bmp` directory. The file name is 'u', followed by the four-digit alt-key code, followed
by a friendly name for the symbol. If you need to add new fonts, put the details in the `doc/spec.txt` file (using the same
format as the other specified symbols) then run `bin/gen.sh`. This will generate a bitmap file for you in the `bmp`
directory from the `etc/template.bmp` file. You need to edit the bitmap file so that it actually contains the font symbol
that you want, and then run `bin/gen.sh` again. This second pass of the code-generation script will write a C header file
in the `ino/Symbol_Keyboard/gen/` directory based on the bitmap data given in the bitmap file. It will also update the
`declare.c` and `header.h` files in the same directory.

The generated `declare.c` file shows you how buttons for each symbol can be declared. You can copy these examples into the
setup() routine in your Arduino project. You need to declare exactly 12 buttons (no more, no less). Note that this
`declare.c` file is just a helper for you, it won't actually work as-is and isn't meant to be used directly, it's just there
so you can copy and paste any button declarations you would like to use.

The generated `header.h` file includes all of the generated font bitmaps. As a convenience you can just include this header
to include all the symbols that are available in your program. However your program will only ever have call to use twelve
of these, so if you're low on flash memory you can create an alternative header file which only includes the bitmaps you
actually use. You can use the `header.h` file as a basis, just delete everything except for the twelve fonts you actually
want. (You need to keep the line which includes the `avr/pgmspace.h` header too.)

All the other files in the `ino/Symbol_Keyboard/gen/` directory are font bitmaps. This is binary data that describes the
symbol in a format the graphics library can use. As mentioned above these fonts are monochrome 64x64 pixel bitmaps with a
one pixel border. These header files are automatically generated by the `bin/gen.sh` script using the bitmap files in the
`bmp` directory as a basis.

The main code for this application is in `ino/Symbol_Keyboard/Symbol_Keyboard.ino` which is described with its own comments.

Good luck!

If you get stuck or have questions you can find me on the web over here:

- https://www.inthelabwithjayjay.com/

Cheers,
Jay Jay.
