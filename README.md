
## LSB STEGANOGRAPHY FOR SECURE DATA HIDING
  Developed an image-based steganography system using the Least Significant Bit (LSB) technique to hide encrypted text data within RGB images. The system preserves image     quality while enabling accurate data extraction, demonstrating secure communication, bit-level manipulation, and low-level data processing.
## Skills Learned

* Understanding of steganography concepts and data hiding techniques
* Bit-level manipulation and binary data handling
* Image processing fundamentals (RGB pixel manipulation)
* Secure data embedding and extraction techniques
* Low-level data processing and logical problem-solving

---

## Tools Used

* **C / C++ ** for implementation *(use the one you actually used)*
* **Linux / Linux environment** for development and testing
---

## Steps

Below are the key steps involved in implementing the LSB steganography system:

---

### 1. Input Image and Message Selection

In this phase, a cover image (RGB format) and a secret text message are selected. The message is optionally encrypted before embedding to enhance security.

*Ref 1: Original Cover Image*
This image shows the original RGB image used as the carrier for hidden data.
---

### 2. Message Encryption and Binary Conversion

The secret message is encrypted and converted into binary format. Each character is represented in bits to prepare it for LSB embedding.

*Ref 2: Binary Message Representation*
This screenshot illustrates the encrypted message converted into binary form.

---

### 3. LSB Data Embedding

Binary data is embedded into the least significant bits of the RGB pixel values. This ensures that visual distortion remains imperceptible.

*Ref 3: LSB Embedding Process*
This image demonstrates how message bits are embedded into pixel LSBs.

---

### 4. Stego Image Generation

After embedding the data, the modified image (stego image) is generated. The stego image appears identical to the original image to the human eye.

*Ref 4: Stego Image Output*
This screenshot shows the final stego image with hidden data.

---

### 5. Data Extraction and Decryption

The embedded message is extracted by reading the LSBs of the stego image pixels. The binary data is reconstructed, decrypted, and converted back to readable text.

*Ref 5: Extracted Message Output*
This image displays the successfully extracted secret message.

---

## Conclusion

This project successfully demonstrates how **LSB steganography combined with encryption** can be used for secure and covert communication. It provides practical insight into data hiding techniques, image processing, and low-level bit manipulation, making it highly relevant to cybersecurity, digital forensics, and secure communication systems.

---

