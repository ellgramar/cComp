#load files
    
def rem(l):
    l.remove('else')
    l.remove('if')
    l.remove('(op')
    l.remove('==')
    op = l.pop(0)
    list = op.split(")")
    op = ''.join(list)
    return l,op
def parse(l):
    list = l.split(" ")
    list, op = rem(list)
    list = ' '.join(list)
    string = "case "+op+":\n"+list+"break;"+"\n"
    return string
def main():
    #open files
    with open('ifElse.txt','r') as rf:
        with open('switchCase.txt','w') as wf:
            for line in rf:
                wf.writelines(parse(line))

            #wf.writelines(parse(rf.readlines()))
    

    #load line from file into var



    

#main
if __name__ == "__main__":
    main()