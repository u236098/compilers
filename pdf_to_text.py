import PyPDF2
import os

def convert_pdf_to_txt(pdf_path, txt_path):
    """
    Convert a PDF file to a text file.
    
    Args:
        pdf_path: Path to the input PDF file
        txt_path: Path to the output text file
    """
    try:
        # Open the PDF file
        with open(pdf_path, 'rb') as pdf_file:
            # Create a PDF reader object
            pdf_reader = PyPDF2.PdfReader(pdf_file)
            
            # Get the number of pages
            num_pages = len(pdf_reader.pages)
            print(f"Converting {pdf_path} ({num_pages} pages)...")
            
            # Extract text from all pages
            text_content = []
            for page_num in range(num_pages):
                page = pdf_reader.pages[page_num]
                text_content.append(f"\n--- Page {page_num + 1} ---\n")
                text_content.append(page.extract_text())
            
            # Write to text file
            with open(txt_path, 'w', encoding='utf-8') as txt_file:
                txt_file.write(''.join(text_content))
            
            print(f"✓ Successfully converted to {txt_path}")
            return True
            
    except Exception as e:
        print(f"✗ Error converting {pdf_path}: {str(e)}")
        return False

def main():
    # List of PDF files to convert
    pdf_files = [
        "COMP-T2_lexical_analysis.pdf",
        "COMP-T3_parsing.pdf",
        "COMP-T4_bottomup_parsing.pdf"
    ]
    
    # Convert each PDF
    for pdf_file in pdf_files:
        if os.path.exists(pdf_file):
            # Create output filename
            txt_file = pdf_file.replace('.pdf', '.txt')
            convert_pdf_to_txt(pdf_file, txt_file)
        else:
            print(f"✗ File not found: {pdf_file}")

if __name__ == "__main__":
    main()
