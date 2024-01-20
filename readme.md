# Elan compiler elancc

Elan [[1]](https://de.wikipedia.org/wiki/ELAN_%28Programmiersprache%29) [[2]](https://en.wikipedia.org/wiki/ELAN_%28programming_language%29)  compiler for Linux/Unix by Marc Seutter using gcc as backend.

## Supported targets

    CPU          OS              Comment
    i686         Linux 2.2.19    Slackware
    i686         Linux 2.2.10    Suse 6
    i686         Linux 2.4.5     Debian
    amd64        FreeBSD 4.4/4.6 SMP
    sparc-sun4m  Solaris 2.8
    sparc-sun4u  Solaris 2.9
    alpha        NetBSD

## Building elancc

First build Mimir:

    $ cd mimir-3.1
    $ ./confgure
    $ make
    $ sudo make install

Then build elancc:

    $ cd elan-1.11
    $ ./configure
    $ make
    $ sudo make install

## Using elanncc

Invoke `elancc` on your Elan source files:

    $ elancc hello.e

This creates the executable `hello`. 

You find Elan example programs in [elan-1.11/exp](elan-1.11/exp).

Get more information by `elancc --help` or `man elancc`


## Documentation

See [The Manuals](elan-1.11/manuals) at [elan-1.1/manuals](elan-1.11/manuals).


## Literature

- Günter Hommel, Joachim Jäckel, Stefan Jähnichen, Karl Kleine, Wilfried Koch, Kees Koster: ELAN-Sprachbeschreibung. Akademische Verlagsgesellschaft, Wiesbaden 1979, ISBN 3400003840.
- Rainer Hahn, Peter Stock: ELAN-Handbuch. 2., korr. Auflage. Akademische Verlagsgesellschaft, Wiesbaden 1982, ISBN 3-400-00499-5 
- L.H. Klingen, J. Liedtke: Programmieren mit ELAN. Teubner Verlag, Stuttgart 1983, ISBN 3519025078.
- L.H. Klingen, J. Liedtke: ELAN in 100 Beispielen. Teubner Verlag, Stuttgart 1985, ISBN 3519025213.
- C.H.A. Koster: Top-Down Programming with Elan. Ellis Horwood, 1987, ISBN 0745801870.

## Original Readme by Marc Seutter

> This is the most recent compiler version for ELAN. It is
> strictly intended for Linux/Unix. It assumes that you have
> gcc installed on your computer, as it uses the compiler
> driver to assemble and link. The Elan compiler can generate
> assembler code for Intel x86, AMD 64, Sparc (<V9), Alpha
> as well as VAX11 (not tested; anyhow who has a VAX11 still
> in operation nowadays). Several runtime extensions are also
> available namely the socket, experimental X11/Xaw, and
> experimental thread library.
> 
> To compile the new Elan compiler, you must also download
> the datastructure generator packet mimir from
> ftp://ftp.cs.ru.nl/pub/vb/mimir
> 
> Good luck, Marc Seutter
> (marcs@cs.ru.nl)


Enjoy, Ulrich Hoffmann (uho@xlerb.de)
