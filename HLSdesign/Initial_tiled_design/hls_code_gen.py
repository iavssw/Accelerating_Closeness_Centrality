




def main():
    
    genfile = open ("genfile.txt", 'w')
    
    for i in range(20):
        
        text = "unsigned short C" + str(i) + "[BLOCK_SIZE][BLOCK_SIZE];\n"
        genfile.write(text)
        text = "unsigned short A" + str(i) + "[BLOCK_SIZE][BLOCK_SIZE];\n"
        genfile.write(text)
        text = "unsigned short B" + str(i) + "[BLOCK_SIZE][BLOCK_SIZE];\n"
        genfile.write(text)

        genfile.write("\n")

    for i in range(20):
        text = "FW_core(C" + str(i) + ", A"  + str(i) + ", B" + str(i) + ");\n"
        genfile.write(text)

    genfile.write("\n")

    for i in range(20):
        text = "get_data(address["+ str(i) + "], C" + str(i) + ", A" + str(i) + ", B" + str(i) + ", adj_mat);\n"
        genfile.write(text)

    genfile.write("\n")

    for i in range(20):
        text = "return_data(address["+ str(i) + "], C" + str(i) + ", adj_mat);\n"
        genfile.write(text)


    genfile.close()









if __name__ == '__main__':
    main()