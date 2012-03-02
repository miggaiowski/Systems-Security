import os
import sys
import argparse

def harvest(path, exts):
    files_found = []
    words_found = set()
    for top, dirs, files in os.walk(path):
        for file in files:
            if file.split('.')[-1] in exts:
                files_found.append(os.path.join(top, file))
    for file in files_found:
        f = open(file)
        reading = f.read()
        words = set(reading.split())
        words_found.update(words)
    return words_found

def usage():
    print "Usage:", sys.argv[0], "[-e <ext1:ext2...>] -d <dir_to_traverse> -o <out_file>"    
    

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Harvest words in files')
    parser.add_argument('-e', metavar="<ext1:ext2:..>", help="Extensions to open, separated by :")
    parser.add_argument('-d', metavar="<dir>", default='.', help="Dir to traverse")
    parser.add_argument('-o', metavar="<outfile>", help="Output file")

    args = parser.parse_args()
    if not os.path.exists(args.d):
        print "Path not found"
        sys.exit(1)
    exts = 'txt:text'
    if args.e:
        exts = args.e
    exts.split(':')
    words = harvest(args.d, exts)

    if (args.o and args.o != "-"):
        f = open(args.o, 'w')
        for w in words:
            f.write(w+"\n")
        f.close()
    else:
        for w in words:
            print w

