#load files
def open_files():
    
    try:
        rfile = open('ifElse.txt','r')
    except:
        print("cannot open ifElse.txt")
    try:
        wfile = open('switchCase.txt','a')
    except:
        print("cannot create or open switchCase.txt")
    return rfile, wfile




#load line from file into var
#parse line into case and funct
#combine case and function into proper format and store as var
#output each line to file
#close files

def main():
    rfile, wfile = open_files()
    print(rfile)

#main
if __name__ == "__main__":
    main()