echo 'char shader[] = {' > $2
hexdump -e '/1 "  0x%02x,\n"' $1 >> $2
echo '  0x00' >> $2
echo '};' >> $2
