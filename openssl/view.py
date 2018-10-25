import os
import sys


def toHex(b):
    s = ""
    x1 = (b & 0xF0) >> 4
    x2 = b & 0x0F
    if x1 < 10:
        s += chr(x1 + ord('0'[0]))
    else:
     s += chr((x1 - 10) + ord('A'))
    if x2 < 10:
        s += chr(x2 + ord('0'))
    else:
        s += chr((x2 - 10) + ord('A'))
    return s


def toHex8(n):
    s = ""
    x1 = (n & 0xF0000000) >> 28
    x2 = (n & 0xF000000) >> 24
    x3 = (n & 0xF00000) >> 20
    x4 = (n & 0xF0000) >> 16
    x5 = (n & 0xF000) >> 12
    x6 = (n & 0xF0) >> 8
    x7 = (n & 0xF0) >> 4
    x8 = n & 0x0F
    if x1 < 10:
        s += chr(x1 + ord('0'))
    else:
        s += chr((x1 - 10) + ord('A'))
    if x2 < 10:
        s += chr(x2 + ord('0'))
    else:
        s += chr((x2 - 10) + ord('A'))
    if x3 < 10:
        s += chr(x3 + ord('0'))
    else:
        s += chr((x3 - 10) + ord('A'))
    if x4 < 10:
        s += chr(x4 + ord('0'))
    else:
        s += chr((x4 - 10) + ord('A'))
    s += " "
    if x5 < 10:
        s += chr(x5 + ord('0'))
    else:
        s += chr((x5 - 10) + ord('A'))
    if x6 < 10:
        s += chr(x6 + ord('0'))
    else:
        s += chr((x6 - 10) + ord('A'))
    if x7 < 10:
        s += chr(x7 + ord('0'))
    else:
        s += chr((x7 - 10) + ord('A'))
    if x8 < 10:
        s += chr(x8 + ord('0'))
    else:
        s += chr((x8 - 10) + ord('A'))
  
    return s

def hex_print(fname, fsize):
    try:
        file2 = open(fname, "rb")
        n = 0
        dum = ""
        for i in range(fsize):
            if n % 16 == 0:
                sys.stdout.write("%s: " % toHex8(n & 0xFFFFFFFF))
             
            if n % 16 != 8:
                sys.stdout.write(" ")
            else:
                sys.stdout.write("-")

            a = file2.read(1)
            sys.stdout.write("%s" % toHex(int(ord(a))))
                
            if int(ord(a)) < int(ord(' ')) or int(ord(a)) & 0x80 != 0:
                dum += "."
            else:
                dum += a
                
            if n > 0 and n % 16 == 15:
                sys.stdout.write("  |%s|\n" % dum)
                dum = ""

            n += 1

        if n > 0 and n % 16 > 0:
            for i in range(16 - (fsize % 16)):
                sys.stdout.write("   ")
            sys.stdout.write("  |%s" %dum)
            for i in range(16 - (fsize % 16)):
                sys.stdout.write(" ")
            sys.stdout.write("|\n")
            dum = ""
    except IOError: 
        print "Error: the file \"%s\" cannot be read more." % fname
    finally:
        file2.close()


if __name__ == "__main__":
    file_list = [ "plain", "plain.enc", "plain.enc.dec" ]
    
    for fname in file_list:
        if not os.path.exists(fname):
            print "The file '%s' does not exist.\n" % fname
            continue
        elif os.path.isdir(fname):
            print "%s is a directory.\n" % fname
            continue

        fsize = os.path.getsize(fname)

        if fsize != None:
            print "The size of the file \"%s\" is %d." % (fname, fsize)
            if fsize == 0:
                sys.exit(1)
        print "-------------------------------------------------------------------------------"
        hex_print(fname, fsize)
        print "-------------------------------------------------------------------------------"
        print
