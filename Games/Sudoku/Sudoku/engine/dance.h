#ifndef DANCE_H
#define DANCE_H

struct data_object 
{
	struct data_object *up, *down, *left, *right;
	struct column_object *column;
};

struct column_object 
{
	struct data_object data; /* must be the first field */
	size_t size;
	char *name;
};

struct dance_matrix 
{
	size_t nrows, ncolumns;
	struct column_object *columns;
	struct column_object head;
};

class DLX
{
// Construction
public:
	DLX();
	virtual ~DLX();

// Operations
public:
	int dance_init(struct dance_matrix *m,
				   size_t rows, 
				   size_t cols, 
				   const int *data);
//int DLX::print_sudoku_result(size_t n, struct data_object **sol)

	int dance_solve(struct dance_matrix *m,
					int (*f)(size_t, 
					struct data_object **),
					int max_solutions);
	//int dance_print(struct dance_matrix *m);
	
					
	int sudoku_solve(int grid[9][9]);		// returns 0	- Puzzle unsolvable
											// returns 1	- Puzzle solved
											// returns >1	- Puzzle invalid


	static int print_sudoku_result(size_t n, struct data_object **sol);
	//int print_result(size_t n, struct data_object **sol);

// Implementation
protected:
	void *Malloc(size_t);
	int dance_init_named(struct dance_matrix *m,
						 size_t rows, 
						 size_t cols, 
						 const int *data, 
						 char **names);
	int dance_addrow(struct dance_matrix *m,
					 size_t nentries, size_t *entries);
#if 0  // -----------------------------------------------------------
	int dance_addrow_named(struct dance_matrix *m,
						   size_t nentries, char **names);
#endif // -----------------------------------------------------------
	int dance_free(struct dance_matrix *m);
	int dancing_search(size_t k, 
					   struct dance_matrix *m,
					   int (*f)(size_t, 
					   struct data_object **),
					   struct data_object **solution,
					   int max_solutions);
	void dancing_cover(struct column_object *c);
	void dancing_uncover(struct column_object *c);

	static int m_grid[9][9];
};

#endif //DANCE_H
