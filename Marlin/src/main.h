
void can_setup(void);
void can_proc_in_loop();
bool can_data_available();
int can_read_serial();

class SerialToCAN
{
  public:
    size_t write(uint8_t b);
	size_t write(const char *str)			{ return write(str, strlen(str)); }
	//virtual size_t write(const uint8_t *buffer, size_t size);
	// virtual int availableForWrite(void)		{ return 0; }
	// virtual void flush()				{ }
	size_t write(const char *buffer, size_t size);
	size_t print(const String &s);
	size_t print(char c)				{ return write((uint8_t)c); }
	size_t print(const char s[])			{ return write(s); }

	size_t print(uint8_t b)				{ return printNumber(b, 10, 0); }
	size_t print(int n)				{ return print((long)n); }
	size_t print(unsigned int n)			{ return printNumber(n, 10, 0); }
	size_t print(long n);
	size_t print(unsigned long n)			{ return printNumber(n, 10, 0); }

	size_t print(unsigned char n, int base)		{ return printNumber(n, base, 0); }
	size_t print(int n, int base)			{ return (base == 10) ? print(n) : printNumber(n, base, 0); }
	size_t print(unsigned int n, int base)		{ return printNumber(n, base, 0); }
	size_t print(long n, int base)			{ return (base == 10) ? print(n) : printNumber(n, base, 0); }
	size_t print(unsigned long n, int base)		{ return printNumber(n, base, 0); }

    size_t println(void);
	size_t println(const String &s)			{ return print(s) + println(); }
	size_t println(char c)				{ return print(c) + println(); }
	size_t println(const char s[])			{ return print(s) + println(); }
  private:
	size_t printNumber(unsigned long n, uint8_t base, uint8_t sign);
};
extern SerialToCAN SerialCAN;

