# 
#  check that the input list is nicely unique
#
import sys

seen = dict()


filename = 'posix5.stderr.txt'
with open(filename, 'r') as f:
    for this_line in f:
        this_line = this_line.strip()
        if this_line.find("="):
            # interesting: 8739237276630802765 -> hashed Mandalay
            fields = this_line.split('=')
            hash = fields[0]
            location = fields[1]
            if hash in seen:
                if seen[hash] != location:
                    print(f"hash collision : {hash} is {location} and {seen[hash]}") 
                    sys.exit(1)
            else:
                print(f"Storing {hash} for {location}")
                seen[hash] = location
        else:
            print(this_line)

