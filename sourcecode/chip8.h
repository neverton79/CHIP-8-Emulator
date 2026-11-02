class Chip8 {
    public: 

        Chip8();
        ~Chip8();

        bool drawFlag;

        void loadROM(const char* filename);
        void emulateCycle();


        unsigned char gfx[64 * 32];
        unsigned char key[16];

    private:

        unsigned short m_addressI;
        unsigned short m_programCounter;
        unsigned short m_stackPointer;
        unsigned short m_opCode;
        
        unsigned char m_gameMemory[4096];
        unsigned char m_registers[16];
        unsigned short m_stack[16];

        unsigned char m_delayTimer;
        unsigned char m_soundTimer;

        void initialize();
        void drawAt(unsigned char x, unsigned char y, unsigned char height);
};