import sys
if __name__ == "__main__":

    # check if port number is provided
    if len(sys.argv) != 2:
        print('Usage: python %s <PortNumber>' % (sys.argv[0]))
        sys.exit()
        
    print(sys.argv[1])
    