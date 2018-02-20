#include<iostream>
#include<stdio.h>
#include<fstream>
#include<cstdlib>
#include"huffman.h"
#include"HCbitstream.h"
#define DEBUG

const int HCcode_tree::char_lett = 256;
const int HCfrtable::char_lett = 256;
const int char_lett = 256;
const int HCcode_tree::max_fr = 1000000000; //nine zeros 
const int char_size = 8;

//===============fr_table functions==================

HCfrtable::HCfrtable() {
	fr = new unsigned[char_lett];
	for(int i = 0; i<char_lett; i++) fr[i] = 0;
		
}

HCfrtable::~HCfrtable() {
	delete[] fr;
}

//===============HCharrray===============

HCharray::HCharray(const int size_) {
	size = size_;
	dat = new char[size];
	for(int i = 0; i<size; i++) dat[i] = 0;
	last_recorded = 0;
}
//copy constructor
HCharray::HCharray(const HCharray &a) {
        size = a.size;
        dat = new char[size];
        for(int i = 0; i<size; i++) dat[i] = a.dat[i];
        last_recorded = a.last_recorded;
    
}
//operator =
const HCharray& HCharray::operator=(const HCharray &a) {
        if(this == &a) return *this;
        delete [] dat;
        size = a.size;
        dat = new char[size];
        for(int i = 0; i<size; i++) dat[i] = a.dat[i];
        last_recorded = a.last_recorded;
        return *this;
    
}
void HCharray::size_inc() {
	char *n; n = new char[size+1];
	for(register int i = 0; i<size; i++) {
		n[i] = dat[i];
	}
	n[size] = 0;
	delete [] dat;
	dat = n;
	last_recorded = 0;
	size++;
	//std::cout<<"I've increased the size\n"<<"this = "<<this;
}

void HCharray::add(char s) {
	if((s!=0)&&(s!=1)) {
		HCexcept ups(HCE_KUKU, "addtoenderror");
		throw ups;
	}
	//I write new bit to the beginning
	if(last_recorded == char_size) {
		size_inc();
		dat[size-1] = s<<(char_size-1);
	}
	else {
		//I write bits from left to right
		dat[size-1] = (dat[size-1]) | (s<<(char_size-last_recorded-1));
	}
	last_recorded++;
	//std::cout<<"I've added "<<(int)s<<"\n";	
}

void HCharray::reverse() {
	//std::cout<<"before reverse "; dprint(std::cout); std::cout<<"\n";
	HCharray bingo(1);
	register char c;
	for(register int i = size-1; i>=0; i--) {
		for(register int j = ((i==size-1) ? (char_size - last_recorded) : 0); 
				 j<char_size; j++) {
			c = (dat[i] >> j) & ((char)1); //std::cout<<(int)c;
			bingo.add(c);
		}
	}
	//then I copy data
	for(register int i = 0; i<size; i++) {	
		dat[i] = bingo.dat[i];
	}
	//std::cout<<"after "; dprint(std::cout); std::cout<<"\n"; 

}

void HCharray::dprint(std::ostream &out) {
	//char c;
	for(int i = 0; i<size; i++) {
		for(int j = char_size-1; 
			j>=((i==size-1) ? char_size - last_recorded : 0); j--) {
			if(dat[i] & (1<<j)) { out<<1; }
			else {out<<0; }		
		}
	}
}
//===============code_tree_node_list descriptions==============


HCcode_tree::node_list::node_list() {
	head = NULL;
}

HCcode_tree::node_list::~node_list() {
	if(!head) return; //if the list is already empty, it does nothing
	
	list_item *c = head;
	list_item *d = c;
	while(!c) {
		c = d->next;
		delete d;
		d = c;
	}
}

void HCcode_tree::node_list::push(HCtrnode *a) {
	list_item *f;
	f = new list_item;
	f->node = a;

	//where to place the new element?
	//empty list case
	if(!head) {
		head = f;
		head->next = NULL;
		return;
	}
	//non'empty list case
	list_item *g;
	for(g = head; g->next; g = g->next) {
		if( ( (*g) <= (*f) ) && ( (*f) < (*(g->next)) ) ) {
			//perfect plase to place
			f->next = g->next;
			g->next = f;
			return;
		}
	}
	//in this case f should be in the end
	//g->next==NULL
	f->next = g->next;
	g->next = f;
}

HCtrnode* HCcode_tree::node_list::extract() {
	if(!head) return NULL; 
	list_item *d; d = head;
	head = head->next;
	HCtrnode* t = d->node;
	delete d;
	return t;
	 
}

bool HCcode_tree::node_list::onlyone() {
	if(isEmpty()) return false;
	return !(head->next);
}

//=============HCcode_tree==================

//marvellous constructor
HCcode_tree::HCcode_tree(HCfrtable &ftr) {
	node_list L; //my list
	//leaves creation
	HCtrnode *t;
	for(int i = 0; i<char_lett; i++) {
		t = new HCtrnode;
		t->s = i;
		t->fr = ftr.fr[i];
		t->left = t->right = NULL;
		L.push(t);
		leaves.b[i] = t;
	}

	//then I create the big tree
	if(L.isEmpty()) {
		HCexcept ups(HCE_KUKU, "Just an error\n");
		throw ups;
	}
	HCtrnode *u, *v, *just; 
	while(!(L.onlyone())) {
		//I extract two the least frequent leaves
		u = L.extract();
		v = L.extract();
		if( ((u->fr) > max_fr) || ((v->fr) > max_fr) ) {
			HCexcept ups(HCE_BIG_TEXT, "big text");
			throw ups;
		}
		//Then I create new leaf
		just = new HCtrnode;
		just->fr = (u->fr) + (v->fr);
		just->left = u;
		just->right = v;
		u->parent = just;
		v->parent = just;
		L.push(just);		
	}
	//then I should extract my root
	root = L.extract();
	if(!L.isEmpty()) {
		HCexcept ups(HCE_KUKU, "tree has not constructed completely");
		throw ups;
	}
	
}

void HCcode_tree::cheatting_destructor(HCtrnode *a) {
        if(!a) return;
        cheatting_destructor(a->left);
        cheatting_destructor(a->right);
        delete a;
}

HCcode_tree::~HCcode_tree() {
	cheatting_destructor(root);
}

//TODO: suffer a lot from this function
//TODO: enjoy
HCharray* HCcode_tree::code(const char a){
	HCtrnode *u;

	//getting the leaf corresponding to the letter a
	u = leaves.b[(unsigned char) a];
	if(u==NULL) {
		HCexcept ups(HCE_KUKU, "CODE_TREE_CODE:: I can't find the needed leaf");
		std::cerr<<"a = "<<(unsigned char)a<<" "<<(int)(unsigned char)a<<"\n";
		throw ups;
	}
	
	return route(u);	
	
}
HCharray* HCcode_tree::route(HCtrnode *u) {
        HCtrnode *v;
        HCharray *ans = new HCharray;
	//travelling through the root
	char symb;
	while(u!=root) {
		if(u==NULL) {
			HCexcept ups(HCE_KUKU, "CODE_TREE::CODE: where're parents??");
			throw ups;
		}
		v = u->parent;	
		if(u == v->left) {
			symb = 0;
		} 
		else {
			if(u == v->right) {
				symb = 1;	
			}
			else {
				HCexcept ups(HCE_KUKU, "CODE_TREE_CODE: Strange error");
				throw ups;
			}
		}	
		ans->add(symb);
		u = v;
	}
	//Then I recognize that the code is inversed:
	//I would start from root, not leaf
	ans->reverse();
	return ans;
}

//===================HCtree_walk======================
HCtree_walk::HCtree_walk(HCcode_tree *t_) {
	t = t_;
	root = c = t->root;	
}

bool HCtree_walk::isleaf() {
	return ((c->left == NULL)&&((c->right)==NULL));
}

void HCtree_walk::walk(char dr) {
	const HCtrnode *d;
	switch(dr) {
		case 0:
			d = c->left;
			break;
		case 1:
			d = c->right;
			break;
		default:	
			HCexcept ups(HCE_KUKU, "tree_walk: Uncorrect input data");
                        std::cerr<<"Input = "<<(int)dr<<" "<<"\n";
			throw ups;
	}
	if(!d) {
		HCexcept ups(HCE_TREE_WALKING, "tree_walk: can't move");
		throw ups;
	}

	c = d;

}

void HCtree_walk::gotoparent() {
	if(c==root) {
		HCexcept ups(HCE_WALK_ROOT, "tree_walk: there's no root parent");
		throw ups;
	}
	c = c->parent;
}

char HCtree_walk::read() {
	if(!isleaf()) {
		HCexcept ups(HCE_WALK_READ, "tree_walk: I can't read non-leaves");
		throw ups;
	}
	return c->s;
}


//======================non-adaptive method==================

void HCnon::count_freq() {
	std::ifstream in;
	char x = 0;
	in.open(inname, std::ios::binary);
	//fin = fopen(inname, "rb");
	if(!in) {
		std::cerr<<"KUKU\n";
		HCexcept ups(HCE_OPEN_FILE, "I can't open an input file");
		throw ups;
	}

    in.get(x);
	while(!in.eof()) {
        frt.fr[(unsigned char)x]++; //TODO dangerous
        in.get(x);
	}
	in.close();
	//fclose(fin);
}

//TODO calculations related to the last symbol
void HCnon::encode(const char *inname_, const char *outname_) {
	inname = inname_;
	outname = outname_;
	
	count_freq();
		
	in.open(inname, std::ios::binary);
	if(!in) {
		HCexcept ups(HCE_OPEN_FILE, "I can't open an input file");
		throw ups;
	}

	//haffman codes array
	HCharray *codes[char_lett];
	{
		//marvellous tree creation
		HCcode_tree ma(frt);
		
		//then I record symbol codes
		for(int i = 0; i<char_lett; i++) codes[i] = ma.code( (char)i );
		std::cout<<"Tree: created\n";
	}
	//now I don't need to hold the whole tree
	//I've founded huffman codes

	//out.open(outname, std::ios::trunc | std::ios::binary);
	HCofstream out(outname);
	if(!out.success()) {
		HCexcept ups(HCE_OPEN_FILE, "I can't open an output file");
		throw ups;
	}
	//which codes have I received?
	#ifdef DEBUG
	std::ofstream debug;
	debug.open("debug.txt", std::ios::trunc);
	for(unsigned i = 0; i<char_lett; i++) {
		if(frt.fr[i]) {
			debug<<i<<" "<<frt.fr[i]<<" "; codes[i]->dprint(debug);
			//debug<<" ECL: "<<(((codes[i]->size)-1)*8+(codes[i]->last_recorded));
			debug<<"\n";
		}
	}
	debug.close();
	#endif
	//I have to send frequencies
	out.write_array_int(frt.fr, frt.char_lett);

	//then I accurately write down codes
	register unsigned char buf = 0; //buffers
	register int bit = 0; //recorded bits in buf1 and buf2 
	char x; //current letter

    //.init();
	HCharray *xcode; //code of x
    in.get(x);
	while(!in.eof()) {

		xcode = codes[(unsigned char)x];
                #ifdef DEBUG
                //std::cout<<"code of "<<x<<": ";
                //xcode->dprint(std::cout);
                //std::cout<<"\n";
                #endif
		for(register int i = 0; i<(xcode->size); i++) {
			//I assume the buf2 is now empty
	        	buf = xcode->dat[i]; 
			bit= (i!=((xcode->size)-1)) ? char_size : xcode->last_recorded; 
			for(register int j = 0; j<bit; j++) {
				//std::cout<<(unsigned)buf<<" "<<((unsigned)(buf>>(char_size-1)))<<"\n";
				out.writeBit(buf>>(char_size-1));
				buf = buf<<1;
			}
			//std::cout<<"!\n";
						
		}
        in.get(x);
		
	}
    
	//BINGO
    out.close();
	in.close();
	//fclose(fin);
	//remove the HCharrays
	for(int i = 0; i<char_lett; i++) delete codes[i];

	std::cout<<"File compessing: successful\n";
}

void HCnon::decode(const char *inname_, const char *outname_) {
	inname = inname_;
	outname = outname_;
	//char useful=8; //for the last symbol	

	HCifstream in(inname);
	
	if(!in.success()) {
                HCexcept ups(HCE_OPEN_FILE, "I can't open an input file");
                throw ups;
        }

	out.open(outname, std::ios::binary | std::ios::trunc);
	if(!out) {
                HCexcept ups(HCE_OPEN_FILE, "I can't open an output file");
                throw ups;
        }
	
	
	//frequency table reading
	in.read_array_int(frt.fr, frt.char_lett);
	//for(int i = 0; i<char_lett; i++) if(frt.fr[i]) std::cout<<frt.fr[i]<<"\n";
	//tree restoring
	HCcode_tree ma(frt);
	HCtree_walk w(&ma);
	w.toroot(); //set the tree walker

    in.init();
	char buf = 0;
    buf = in.readBit();
	while(buf !=HC_EXAUSTED_STREAM) {
                //std::cout<<"bylk "<<(int)buf<<"\n";
		//now I'm walking through the code code finding the code
		w.walk(buf);
		if(w.isleaf()) {
            std::cout<<(int)(unsigned char)w.read()<<" ";
			out.put(w.read());
			w.toroot();
		}
        buf = in.readBit();
	
	}
			
	//BINGO
        in.close();
	out.close();
	std::cout<<"File decompressing: successful\n";

}

//======================adaptive tree methods==============


HCtadapt::HCtadapt() : HCcode_tree() {
	//NYT node constructing
	root = new HCtrnode;
	//root->NYT = true; 
        root->fr = 0;
	//root->leaf = true;
	//root->level = 0;
	NYT = root;
    root->left = root->right = NULL;
    root->parent = NULL;
}

HCharray* HCtadapt::code_NYT() {
        return route(NYT);
}
 
bool HCtadapt::never_met(unsigned char x) {
	return leaves.b[(int)x] == NULL;
}
HCtrnode* HCtadapt::pointer(unsigned char x) {
	if(never_met(x)) return NYT;
	return leaves.b[(int)x];
}
void HCtadapt::add_symbol(unsigned char x) {
	HCtrnode *leaf_to_increment = NULL;
	HCtrnode *p  = pointer(x);

	if (p==NYT) {
		HCtrnode *q;
		//p->NYT = false; p->leaf = false;
		//new NYT constructing
		p->left = new HCtrnode;
		q = p->left;
		q->fr = 0;
                NYT = q; //set the new NYT
		//q->NYT = true;
		q->left = q->right = NULL;
		q->parent = p;
		//q->level = p->level + 1;	
		//x node constructing
		p->right = new HCtrnode; 
		q = p->right;
		//q->leaf = true; q->NYT = false;
		q->s = x;
		q->fr = 1;
		//q->NYT = false;
		q->left = q->right = NULL;
		q->parent = p;
		//q->level = p->level+1;
		//add new 
		leaves.b[x] = q;
	}
	else {
		swap_with_block_leader(p);	
		if(sibling(p) == NYT) { //TODO What???
			leaf_to_increment = p;
			p = p->parent;
		}
		
	}
	while(p!=NULL) slide(p);
	if(leaf_to_increment !=NULL) slide(leaf_to_increment);
	
}

void HCtadapt::slide(HCtrnode *&p) {
    if(p==root) {
        (p->fr)++;
        p = NULL; //breaking condition
        return;
    }
    HCtrnode *previous_p = p->parent;
    HCtrnode *q;
	if(isleaf(p) == HC_INTERNAL_NODE) {
		//I find the least node weighing more than (p->fr)+1
                q = find_any_with_this_frequency((p->fr) + 1, HC_LEAF_NODE);
                //if(!q) q = find_any_with_bigger_frequency((p->fr)+1, HC_LEAF_NODE);
                if(q) {
                    q = block_leader(q);
                    swap(p, q);
                }
                (p->fr)++;
                p = previous_p;
	}
	else {
                q = find_any_with_this_frequency((p->fr), HC_INTERNAL_NODE); 
                //if(!q) q = find_any_with_bigger_frequency((p->fr), HC_INTERNAL_NODE);  
                if(q) {
                    q = block_leader(q);
                    swap(p, q);
                }
                (p->fr)++;
                p = p->parent;
        }
	

}

/*
int HCtadapt::table_item::level() {
    HCtrnode *c = a;
    int u = 0;
    while(c!=root) {
        c = a->parent;
        u++;
    }
    return u;
}

void HCtadapt::new_table::push(HCtrnode *a) {
	table_item *f;
	f = new list_item;
	f->node = a;

	//where to place the new element?
	//empty list case
	if(!head) {
		head = f;
		head->next = NULL;
		return;
	}
	//non'empty list case
	list_item *g;
	for(g = head; g->next; g = g->next) {
		if( ( (*g) <= (*f) ) && ( (*f) < (*(g->next)) ) ) {
			//perfect plase to place
			f->next = g->next;
			g->next = f;
			return;
		}
	}
	//in this case f should be in the end
	//g->next==NULL
	f->next = g->next;
	g->next = f;

}

void HCtadapt::new_table::update(HCtrnode *a)  {
    
}

table_item* HCtadapt::new_table::seek(HCtrnode *a) {
        
}
*/

bool HCtadapt::same_block(HCtrnode *p, HCtrnode *q) {
	return ((p->fr)==(q->fr))&&((isleaf(p))==(isleaf(q)));
}

//this function seeks in the cursubtree the node which is in the same block that p
//and has the biggest implict number in thus subtree

//TODO
HCtrnode*  HCtadapt::block_leader_induct(HCtrnode *p, HCtrnode *cur) {
	HCtrnode *t, *u; 
	//the NULL result means this subtree doesn't have any 
	//nodes from the p-block
	if(cur==NULL) return NULL;
	
	//lucky case
	if(same_block(cur, p)) return cur;
	
	//case when I won't find a leader
	if(level(cur) > level(p)) return NULL;
	if((cur->fr) < (p->fr)) return NULL;

	t = block_leader_induct(p, cur ->right);
	u = block_leader_induct(p, cur->left);
	//which one to return?
	if(t == NULL) return u;
	if(u == NULL) return t;
	if(level(t) <= level(u)) return t;
	return u;

			
					
}
HCtrnode* HCtadapt::block_leader(HCtrnode *p) {
	return block_leader_induct(p, root);
}

void HCtadapt::swap_with_block_leader(HCtrnode *p) {
        HCtrnode *q = block_leader(p);
        //then I swap these nodes by pointers changing
        swap(p, q);
        
}

void HCtadapt::swap(HCtrnode *p, HCtrnode *q) {
        if(p==q) return;
        if((p==root) || (q==root)) {
            std::cerr<<"ALERT Root swapping\n";
            return;
        }
        HCtrnode *p_mother = p->parent, *q_mother = q->parent;
        if( (!p_mother) || (!q_mother)) {
                HCexcept ups(787, "Just a relatives error");
                throw ups;
        }
        //regular case: p and are not mother and child
        if( (p_mother != q) && (q_mother != p) ) {
            q -> parent = p_mother;
            p -> parent = q_mother;
            if(p_mother->left == p) {
                    if(q_mother->left == q) {
                            p_mother->left = q;
                            q_mother->left = p;
                    }
                    else {
                        p_mother->left = q;
                        q_mother->right = p;
                        
                    }
            }
            else {
                if(q_mother->left == q) {
                            p_mother->right = q;
                            q_mother->left = p;
                    }
                    else {
                        p_mother->right = q;
                        q_mother->right = p;
                    
                }
            
            }
        }
        else {
            //possibly, I have to move containing data directly
            //?!!?!?!?!?!??!?!?!?!!?!?!?
            //I think I haven't do anything
            //or SUFFER A LOT
            std::cerr<<"ALERT: mother-son swapping\n";
        }
        
    
}

HCtrnode *HCtadapt::sibling(HCtrnode *p) {
    HCtrnode *p_mother = p->parent;
    if(p_mother->left == p) {
            return p_mother->right;
    }
    else {
            return p_mother->left;
    }
}

bool HCtadapt::isleaf(HCtrnode *p) {
        if((p->left == NULL) && ( p->right == NULL)) {
            return HC_LEAF_NODE;
        }
        else {
            return HC_INTERNAL_NODE;
        }
}

int HCtadapt::level(HCtrnode *p) {
        int ans = 0;
        for(ans = 0; p; p=p->parent, ans++);
        return ans;
}

HCtrnode* HCtadapt::find_any_with_bigger_frequency(int fr, bool type) {
            return find_any_with_bigger_frequency_induct(fr, type, root);
}

HCtrnode* HCtadapt::find_any_with_bigger_frequency_induct(int fr, bool type, HCtrnode *q) {
        //I consider some cases
        if(q == NULL) return NULL;
        if(q->fr <=fr) return NULL;
           
        HCtrnode *a, *b;
        a = find_any_with_bigger_frequency_induct(fr, type, q->left);
        if(a) return a;
        b = find_any_with_bigger_frequency_induct(fr, type, q->right);
        if(b) return b;
           
        if(((q->fr) > fr)&&(type == isleaf(q))) return q;
       
        return NULL;
}

HCtrnode* HCtadapt::find_any_with_this_frequency(int fr, bool type) {
            return find_any_with_this_frequency_induct(fr, type, root);
}

HCtrnode* HCtadapt::find_any_with_this_frequency_induct(int fr, bool type, HCtrnode *q) {
        if(q == NULL) return NULL;
        if(q->fr < fr) return NULL;
           
        HCtrnode *p;
        if( ((q->fr) == fr) && (type == isleaf(q))) return q;
        p = find_any_with_this_frequency_induct(fr, type, q->right);
        if(p) return p;
           
        return find_any_with_this_frequency_induct(fr, type, q->left);
}

//================================adaptive huffman coding==============bingo!========
void HCadapt::encode(const char *inname_, const char *outname_) {
        inname = inname_;
	outname = outname_;
    
	in.open(inname, std::ios::binary);
	if(!in) {
		HCexcept ups(HCE_OPEN_FILE, "I can't open an input file");
		throw ups;
	}
	//now I don't need to hold the whole tree
	//I've founded huffman codes

	//out.open(outname, std::ios::trunc | std::ios::binary);
	HCofstream out(outname);
	if(!out.success()) {
		HCexcept ups(HCE_OPEN_FILE, "I can't open an output file");
		throw ups;
	}
	
	//ERROR MUST BE FOUND//ERROR MUST BE FOUND//ERROR MUST BE FOUND//ERROR MUST BE FOUND//ERROR MUST BE FOUND
	//ERROR MUST BE FOUND//ERROR MUST BE FOUND//ERROR MUST BE FOUND//ERROR MUST BE FOUND//ERROR MUST BE FOUND
	HCtadapt make_it_please;
        
        char x;
        bool first_time;
        unsigned char buf;
        int bit;
        HCharray *xcode  = NULL;
        //int bit;
        //I have to make it working
        in.get(x);
        //std::cout<<"I got"<<x<<"\n";
        if(!in.eof()) {
            //in.get(x);
            buf = x;
            for(register int j = 0; j<char_size; j++) {
                        //std::cout<<(unsigned)buf<<" "<<((unsigned)(buf>>(char_size-1)))<<"\n";
                        out.writeBit(buf>>(char_size-1));
                        std::cout<<(int)(buf>>(char_size-1));
                        buf = buf<<1;
            }
            make_it_please.add_symbol(x);
            in.get(x);
        }
        //HELPLESS ME
        
        while(!in.eof()) {
                //std::cout<<"\n==================\n";
                //make_it_please.print_tree1();
                //std::cout<<"\n==================\n";
                //I read a symbol
                
                first_time = make_it_please.never_met(x);
                //if(first_time) std::cout<<"\nbebebebebebe\n";
                //then I have to make a desicion
                if(first_time) {
                        xcode = make_it_please.code_NYT();
                }
                else {
                        xcode = make_it_please.code(x);
                }
                //then I put down codes
                for(register int i = 0; i<(xcode->size); i++) {
               
                    buf = xcode->dat[i];
                    bit= (i!=((xcode->size)-1)) ? char_size : xcode->last_recorded;
                    for(register int j = 0; j<bit; j++) {
                        //std::cout<<(unsigned)buf<<" "<<((unsigned)(buf>>(char_size-1)))<<"\n";
                        out.writeBit(buf>>(char_size-1));
                        buf = buf<<1;
                    }
                //std::cout<<"!\n";
						
                }
                //if the symbol hasn't been met, I write the 8-bit code
                 if(first_time) {
                    buf = x;
                    for(register int j = 0; j<char_size; j++) {
                                    //std::cout<<(unsigned)buf<<" "<<((unsigned)(buf>>(char_size-1)))<<"\n";
                                    out.writeBit(buf>>(char_size-1));
                                    buf = buf<<1;
                    }
                }
                //then I add the new symbol to the tree
                make_it_please.add_symbol(x);
                
                delete xcode;
                in.get(x);
        }
        
        //then I have to close files
        in.close(); out.close();
        std::cout<<"File compressing: successful\n";
        

}

void HCadapt::decode(const char *inname_, const char *outname_) {
        inname = inname_;
	outname = outname_;
	//char useful=8; //for the last symbol	

	HCifstream in(inname);
	
	if(!in.success()) {
                HCexcept ups(HCE_OPEN_FILE, "I can't open an input file");
                throw ups;
    }

	out.open(outname, std::ios::binary | std::ios::trunc);
	if(!out) {
                HCexcept ups(HCE_OPEN_FILE, "I can't open an output file");
                throw ups;
        }
        in.init();
	
	
		
	//for(int i = 0; i<char_lett; i++) if(frt.fr[i]) std::cout<<frt.fr[i]<<"\n";
	//tree restoring
    HCtadapt make_it_please;
    HCtree_walk_adapt w(&make_it_please);
        
    unsigned char buf = 0;
    unsigned char x = 0;
    //in the beginning, there is only an empty tree
    for(register int i = char_size-1; i>=0; i--) {
           buf = in.readBit();
           x = x | (buf<<i);
     }
     std::cout<<(int)x<<"\n";
     out.put(x);
     make_it_please.add_symbol(x);
     buf = in.readBit();
     w.toroot();
     while(buf !=HC_EXAUSTED_STREAM) {
        //std::cout<<"bylk "<<(int)buf<<"\n";
		//now I'm walking through the code code finding the code
		w.walk(buf);
                //std::cout<<"DCMPR: "<<w.isleaf()<<" "<<w.isNYT()<<"\n";
		if(w.isleaf()) {
                        //std::cout<<(int)(unsigned char)w.read()<<" ";
                    
                        //NYT case
                        if(w.isNYT()) {
                                //then I have to read a symbol
                                x = 0;
                                for(register int i = char_size-1; i>=0; i--) {
                                    buf = in.readBit();
                                    x = x | (buf<<i);
                                }
                        }
                        //regular case
                        else {
                            x = w.read();
                            //out.put(x);
                        }
                        out.put(x);
			w.toroot();
                        make_it_please.add_symbol(x);
		}
		
                buf = in.readBit();
                out.flush();
	
	}
			
	//BINGO
        in.close();
	out.close();
	std::cout<<"File decompressing: successful\n";
    
}

//================useless============
void HCtadapt::print_tree(HCtrnode *tree, int deep) {
		if((tree)->left != NULL) {
			print_tree((tree)->left, deep+1);
		}
		
		if((tree)->right != NULL) {
			print_tree((tree)->right, deep+1);
		}
		
		char symbol_re[3];
		sprintf(symbol_re, "%2x", (tree)->s);
		
		if (symbol_re[0] == ' ') {
			symbol_re[0] = '0';
		}
		
                printf("%d 0x%s %c %3d", deep, symbol_re, (tree)->s, (tree)->fr);
		
		if ((tree)->fr == 0) {
			std::cout << " << NYT";
			
		} 
		else if ((tree)->left == NULL) {
			printf(" << 0x%s", symbol_re);
			
		}
		
		std::cout << '\n';
		
		return;
}
