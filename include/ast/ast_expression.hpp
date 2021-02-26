#ifndef ast_declaration_hpp
#define ast_declaration_hpp

#include <string>
#include <iostream>
#include "ast_node.hpp"

class declaration_specifiers : public Root{
  private:
    int type;
    Nodeptr l,r;
    //std::string* string;

  public:
    //case 1, 2
    declaration_specifiers(int type_in, Nodeptr _l, Nodeptr _r) : type(type_in), l(_l), r(_r){}
    // //case 5
    // declaration_specifiers(int type, std::string* string) : type(type_in), string(string_in){}
    // //case 6
    // declaration_specifiers(int type, std::string* string, Nodeptr p) : type(type_in), string(string_in), Nodeptr p(_p){}


};

class init_declarator_list : public Node{
  private:
    Nodeptr l;
    Nodeptr r;
  public:
    init_declarator_list(Nodeptr _l, Nodeptr _r) : l(_l), r(_r){}

};

class init_declarator : public Node{
  private:
    Nodeptr l;
    Nodeptr r;

  public:
    init_declarator(Nodeptr _l, Nodeptr _r) : l(_l), r(_r){}
};

class type_specifier: public Node{
	private:
		int type;
	public:
		type_specifier(int type_in) : type(type_in) {}
		//~type_specifier(){delete type;}
		virtual void print(std::ostream &dst) const override{
			dst << type;
		}
		//this function only checks the tree
 		std::string c() const override{
			switch (type) {
				case 1:
					return "void";
				break;
				case 2:
					return "char";
				break;
				case 3:
					return "short";
				break;
				case 4:
					return "int";
				break;
				case 5:
					return "long";
				break;
				case 6:
					return "float";
				break;
				case 7:
					return "double";
				break;
				case 8:
					return "signed";
				break;
				case 9:
					return "unsigned";
				break;

				default:
				throw std::runtime_error ("Unknow construct");
				break;
			}
		}

		//code-gen python
		virtual void python(std::string &dst){
			//skip, no implementation
		}
};

class struct_or_union_specifier : public Node {
  private:
    int type;
    std::string* string;
    Nodeptr p;
    Nodeptr l;
    Nodeptr r;

  public:
    //case 1
    struct_or_union_specifier(int type_in, std::string* string_in, Nodeptr _p) : type(type_in), string(string_in), p(_p){}
    //case 2
    struct_or_union_specifier(int type_in, Nodeptr _l, Nodeptr _r) : type(type_in), l(_l), r(_r){}
    //case 3
    struct_or_union_specifier(int type_in, Nodeptr _p, std::string* string_in) : type(type_in), p(_p), string(string_in){}

};


#endif