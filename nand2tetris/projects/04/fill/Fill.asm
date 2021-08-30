// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// for(i=0; i<n; i++)
//      draw black pixels
//      at the beginning of row i
//

(KEYLOOP) 
    @SCREEN         // initialise @addr = SCREEN
    D=A       
    @addr
    M=D

    @KBD            // check whether RAM[KBD] != or == 0
    D=M

    @WHITESCREEN    // KBD == 0, goto WHITESCREEN
    D;JEQ
    
    @BLACKSCREEN    // KBD != 0, goto BLACKSCREEN
    D;JNE

    @KEYLOOP
    0;JMP

(BLACKSCREEN)
    @R0              // RAM[RO]=-1 // 111...11 for BLACK
    M=-1
    
    @DRAWSCREEN      // then call DRAWSCREEN and use it
    0;JMP

(WHITESCREEN)
    @R0             // RAM[R0]=0 // for white
    M=0

    @DRAWSCREEN     // then call DRAWSCREEN and use it
    0;JMP

(DRAWSCREEN)

    @addr           // if addr > KBD-1, goto KEYLOOP
    D=M+1           // (the screen is filled 
    @KBD            //  as the SCREEN pixels are
    D=D-A           //  from SCREEN to KBD-1)
    @KEYLOOP        // screen is 512 x 256 = 131 072 pixels 
    D;JGT           // words are 16 bits -> 8192 addresses    

    @R0             // use the value in RAM[R0] = 0 or -1
    D=M
    @addr           // RAM[SCREEN+addr]=0
    A=M
    M=D
    
    @addr           // addr++
    M=M+1

    @DRAWSCREEN
    0;JMP           // goto DRAWSCREEN