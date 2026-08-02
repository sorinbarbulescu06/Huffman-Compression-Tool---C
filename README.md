# Huffman Archiver & Extractor

Command-line utility written in C for bit-level Huffman compression and decompression.

## Project Structure

* src/ - Source code (.c and .h files)
* to_compress/ - Original files to be compressed
* to_decompress/ - Archive files (.bin) to be extracted
* output/ - Destination directory (generated archives or extracted files)
* compress.exe - Compression executable
* decompress.exe - Decompression executable

## Usage

### 1. Compression
Place your files inside `to_compress/` and run:
./compress.exe file1.txt file2.txt ...

If you want all the files from to_compress/ to be archived use:
./compress.exe to_compress/*

The result will be generated as `archive.bin` inside the `output/` folder.

### 2. Decompression
Place your archive inside `to_decompress/`.

* Full extraction (extracts everything):
  ./decompress.exe archivename.bin

* Selective extraction (extracts specific files only):
  ./decompress.exe archivename.bin file1.txt file2.txt ...

The extracted files will appear in the `output/` folder with their original names.