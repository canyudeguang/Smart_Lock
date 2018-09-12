import os.path
import sys

def convertPath(path):
    return path.replace(os.path.sep, '/')

if __name__ == "__main__":
   if len(sys.argv) >= 2:
      print "Attempting to convert windows paths:", sys.argv[1]
      f = open(sys.argv[1], "r+")
      if f:
         content = f.read()
         ConvertedPaths = []

         for winpath in content:
            ConvertedPaths.append(convertPath(winpath))

         String = ''.join(ConvertedPaths)
         String = " ".join(String.split())
         f.seek(0)
         f.write(String)
         f.truncate()
         f.close()
