#include<iostream>


int main()
{
  FILE* file_in;
  if (!(file_in = fopen(source_file_name.c_str(), "r"))) {
    std::cerr << "Cannot open source file: '" << source_file_name << "'." << std::endl;
    return 1;
  }
  // Set file Flex and Yacc will read from.
  yyset_in(file_in);

  // Prepare asm output file.
  std::ofstream asm_out;
  asm_out.open(destination_file_name);

  // Prepare register allocator.
  RegisterAllocator register_allocator;
  // Compile.
  std::vector<const Node*> ast_roots = parseAST();
  compileAst(asm_out, ast_roots, register_allocator);

  // Close the files.
  fclose(file_in);
  asm_out.close();
  return 0;
}
