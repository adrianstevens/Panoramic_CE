/*
   This program is copyright Arthur O'Dwyer, December 2005.
   It is free for all non-commercial use. Please keep it free.

   This program implements Donald Knuth's "dancing links"
   algorithm for finding an exact row covering of a matrix
   of 0s and 1s. Knuth's small example is demonstrated, and
   then an application to Sudoku solving is shown.

   For Knuth's original paper, see
   http://xxx.lanl.gov/PS_cache/cs/pdf/0011/0011047.pdf

   For information on Dancing Links in general, see
   http://en.wikipedia.org/wiki/Dancing_Links
*/

#include "stdafx.h"
#include "dance.h"

int DLX::m_grid[9][9] = { 0 };

/*
   The |USE_HEURISTIC| macro tells the |dance_solve| routine to
   check out the columns with the fewest "1" entries first. The
   same answers will be produced, but possibly in a different
   order, and possibly faster. (It certainly speeds up the solving
   of newspaper Sudoku puzzles!)
*/
#define USE_HEURISTIC 1

DLX::DLX()
{

}

DLX::~DLX()
{

}

int DLX::dance_init(struct dance_matrix *m,
					size_t rows, 
					size_t cols, 
					const int *data)
{
    char **names = (char **) Malloc(cols * sizeof *names);
    size_t i;
    int rc;

    if (cols < 26) 
	{
        const char *alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        for (i=0; i < cols; ++i) 
		{
            names[i] = (char *) Malloc(2);
            sprintf(names[i], "%c", alpha[i]);
        }
    }
    else 
	{
        for (i=0; i < cols; ++i) 
		{
            names[i] = (char *)Malloc(30);
            sprintf(names[i], "%lu", (long unsigned)i);
        }
    }

    rc = dance_init_named(m, rows, cols, data, names);
    for (i=0; i < cols; ++i)
      free(names[i]);
    free(names);
    return rc;
}

int DLX::dance_init_named(struct dance_matrix *m,
						  size_t rows, 
						  size_t cols, 
						  const int *data, 
						  char **names)
{
    size_t i, j;
	
    m->nrows = rows;
    m->ncolumns = cols;
    m->columns = (struct column_object *) Malloc(m->ncolumns * sizeof *m->columns);
    m->head.data.right = &m->columns[0].data;
    m->head.data.left = &m->columns[m->ncolumns-1].data;
	
    for (i=0; i < m->ncolumns; ++i) 
	{
        m->columns[i].name = (char *) Malloc(strlen(names[i])+1);
        strcpy(m->columns[i].name, names[i]);
        m->columns[i].size = 0;
        m->columns[i].data.up = &m->columns[i].data;
        m->columns[i].data.down = &m->columns[i].data;
        if (i > 0)
			m->columns[i].data.left = &m->columns[i-1].data;
        else 
			m->columns[i].data.left = &m->head.data;
        if (i < cols-1)
			m->columns[i].data.right = &m->columns[i+1].data;
        else 
			m->columns[i].data.right = &m->head.data;
		
        for (j=0; j < rows; ++j) 
		{
            if (data[j*cols+i] != 0) 
			{
			/*
			Found one! To insert it in the mesh at the
			proper place, look to our left, and then
			look up that column.
                */
                int ccol;
                struct data_object *newdo = (struct data_object *) Malloc(sizeof *newdo);
                newdo->down = &m->columns[i].data;
                newdo->up = newdo->down->up;
                newdo->down->up = newdo;
                newdo->up->down = newdo;
                newdo->left = newdo;
                newdo->right = newdo;
                newdo->column = &m->columns[i];
				
                for (ccol=i-1; ccol >= 0; --ccol) 
				{
                    if (data[j*cols+ccol] != 0) break;
                }
                if (ccol >= 0) 
				{
                    int crow, cnt=0;
                    struct data_object *left;
                    for (crow=j; crow >= 0; --crow)
						if (data[crow*cols+ccol] != 0) 
							++cnt;
					left = &m->columns[ccol].data;
					for ( ; cnt > 0; --cnt)
						left = left->down;
					newdo->left = left;
					newdo->right = left->right;
					newdo->left->right = newdo;
					newdo->right->left = newdo;
                }
				
                /* Done inserting this "1" into the mesh. */
                m->columns[i].size += 1;
            }
        }
    }
	
    return 0;
}

int DLX::dance_addrow(struct dance_matrix *m, size_t nentries, size_t *entries)
{
    struct data_object *h = NULL;
    size_t i;
	
    for (i=0; i < nentries; ++i) 
	{
        struct data_object *newdo = (struct data_object *) Malloc(sizeof *newdo);
        newdo->column = &m->columns[entries[i]];
        newdo->down = &m->columns[entries[i]].data;
        newdo->up = newdo->down->up;
        newdo->down->up = newdo;
        newdo->up->down = newdo;
        if (h != NULL) 
		{
            newdo->left = h->left;
            newdo->right = h;
            newdo->left->right = newdo;
            newdo->right->left = newdo;
        }
        else 
		{
            newdo->left = newdo->right = newdo;
            h = newdo;
        }
        newdo->column->size += 1;
    }
	
    m->nrows += 1;
    return nentries;
}

int DLX::dance_free(struct dance_matrix *m)
{
    size_t i;
    for (i=0; i < m->ncolumns; ++i) 
	{
        struct data_object *p = m->columns[i].data.down;
        while (p != &m->columns[i].data) 
		{
            struct data_object *q = p->down;
            free(p);
            p = q;
        }
        free(m->columns[i].name);
    }
    free(m->columns);
    return 0;
}

#if 0  // -----------------------------------------------------------
int DLX::dance_print(struct dance_matrix *m)
{
    size_t i;
    struct data_object *p;
	
    for (i=0; i < m->ncolumns; ++i) 
	{
        TRACE("Column %s has %lu elements:\n", m->columns[i].name,
            (long unsigned)m->columns[i].size);
        p = m->columns[i].data.down;
        while (p != &m->columns[i].data) 
		{
            TRACE("  X");
            p = p->down;
        }
        TRACE("\n");
    }
	
    return 0;
}
#endif // -----------------------------------------------------------

int DLX::dance_solve(struct dance_matrix *m,
					 int (*f)(size_t, struct data_object **),
					 int max_solutions)
{
    struct data_object **solution = 
		(struct data_object **) Malloc(m->ncolumns * sizeof *solution);
    int ns = dancing_search(0, m, f, solution, max_solutions);
    free(solution);
    return ns;
}

int DLX::dancing_search(size_t k, 
						struct dance_matrix *m,
						int (*f)(size_t, struct data_object **),
						struct data_object **solution,
						int max_solutions)
{
    struct column_object *c = NULL;
    struct data_object *r, *j;
    int count = 0;
	
    if (m->head.data.right == &m->head.data) 
	{
        return f(k, solution);
    }
	
    /* Choose a column object |c|. */
#if USE_HEURISTIC
    { 
		size_t minsize = m->nrows+1;
		for (j = m->head.data.right; j != &m->head.data; j = j->right) 
		{
			struct column_object *jj = (struct column_object *)j;
			if (jj->size < minsize) 
			{
				c = jj;
				minsize = jj->size;
				if (minsize <= 1) break;
			}
		}
    }
#else
    c = (struct column_object *)m->head.data.right;
#endif
	
    /* Cover column |c|. */
    dancing_cover(c);
	
    for (r = c->data.down; r != &c->data; r = r->down) 
	{
        solution[k] = r;
        for (j = r->right; j != r; j = j->right) 
		{
            dancing_cover(j->column);
        }
        count += dancing_search(k+1, m, f, solution, max_solutions);
        r = solution[k];
        c = r->column;
        for (j = r->left; j != r; j = j->left) 
		{
            dancing_uncover(j->column);
        }
        if (count > max_solutions)
			break;
    }
	
    /* Uncover column |c| and return. */
    dancing_uncover(c);
    return count;
}

void DLX::dancing_cover(struct column_object *c)
{
    struct data_object *i, *j;
	
    c->data.right->left = c->data.left;
    c->data.left->right = c->data.right;
    for (i = c->data.down; i != &c->data; i = i->down) 
	{
        for (j = i->right; j != i; j = j->right) 
		{
            j->down->up = j->up;
            j->up->down = j->down;
            j->column->size -= 1;
        }
    }
}

void DLX::dancing_uncover(struct column_object *c)
{
    struct data_object *i, *j;
	
    for (i = c->data.up; i != &c->data; i = i->up) 
	{
        for (j = i->left; j != i; j = j->left) 
		{
            j->column->size += 1;
            j->down->up = j;
            j->up->down = j;
        }
    }
    c->data.left->right = &c->data;
    c->data.right->left = &c->data;
}

//#define ROWS(a) (sizeof(a)/sizeof(*a))
//#define COLS(a) (sizeof(*a)/sizeof(**a))

#if 0  // -----------------------------------------------------------
int DLX::print_result(size_t n, struct data_object **sol)
{
    size_t i;
    struct data_object *o;
    for (i=0; i < n; ++i) 
	{
        TRACE("Row %d:", (int)i);
        TRACE(" %s", sol[i]->column->name);
        for (o = sol[i]->right; o != sol[i]; o = o->right)
          TRACE(" %s", o->column->name);
        TRACE("\n");
    }
    return 1;
}
#endif // -----------------------------------------------------------

int DLX::sudoku_solve(int grid[9][9])
{
    struct dance_matrix mat;
    int ns = 0;
    size_t constraint[4];
    int rows = 0;
    int cols = 9*(9+9+9)+81;
    /*
	1 in the first row; 2 in the first row;... 9 in the first row;
	1 in the second row;... 9 in the ninth row;
	1 in the first column; 2 in the first column;...
	1 in the first box; 2 in the first box;... 9 in the ninth box;
	Something in (1,1); Something in (1,2);... Something in (9,9)
    */
    int i, j, k;
	
    dance_init(&mat, 0, cols, NULL);
	
	memset(m_grid, 0, sizeof(m_grid));
	
    /*
	Input the grid, square by square. Each possibility for
	a single number in a single square gives us a row of the
	matrix with exactly four entries in it.
    */
    for (j=0; j < 9; ++j) 
	{
        int seen_this_row[9] = {0};
        for (i=0; i < 9; ++i) 
		{
            int box = (j/3)*3 + (i/3);
            if (grid[j][i] != 0) 
			{
                constraint[0] = 9*j + grid[j][i]-1;
                constraint[1] = 81 + 9*i + grid[j][i]-1;
                constraint[2] = 162 + 9*box + grid[j][i]-1;
                constraint[3] = 243 + (9*j+i);
                dance_addrow(&mat, 4, constraint); ++rows;
                seen_this_row[grid[j][i]-1] = 1;
            }
            else 
			{
                for (k=0; k < 9; ++k) 
				{
                    if (seen_this_row[k]) 
						continue;
                    constraint[0] = 9*j + k;
                    constraint[1] = 81 + 9*i + k;
                    constraint[2] = 162 + 9*box + k;
                    constraint[3] = 243 + (9*j+i);
                    dance_addrow(&mat, 4, constraint); ++rows;
                }
            }
        }
    }
	
    ns = dance_solve(&mat, print_sudoku_result, 10);
	
    /*TRACE(_T("There w%s %d solution%s found.\n"), (ns==1)? _T("as") : _T("ere"),
        ns, (ns==1)? _T(""): _T("s"));*/
	
	memcpy(grid, m_grid, sizeof(m_grid));
	
    dance_free(&mat);

	return ns;
}

int DLX::print_sudoku_result(size_t n, struct data_object **sol)
{
    int grid[9][9];
    size_t i, j;
	
    for (i=0; i < n; ++i) 
	{
        int constraint[4];
        int row, col, val;
        row = col = val = 0;
        constraint[0] = atoi(sol[i]->left->column->name);
        constraint[1] = atoi(sol[i]->column->name);
        constraint[2] = atoi(sol[i]->right->column->name);
        constraint[3] = atoi(sol[i]->right->right->column->name);
        for (j=0; j < 4; ++j) 
		{
            if (constraint[j] < 81) 
			{
                row = constraint[j] / 9;
                val = constraint[j] % 9 + 1;
            }
            else if (constraint[j] < 162) 
			{
                col = (constraint[j]-81) / 9;
            }
        }
        grid[row][col] = val;
    }
	
    /*TRACE(_T("Solution:\n"));
    for (i = 0; i < 9; ++i) 
	{
        TRACE(_T("   %d %d %d  %d %d %d  %d %d %d\n"),
			grid[i][0], grid[i][1], grid[i][2], 
			grid[i][3], grid[i][4], grid[i][5], 
			grid[i][6], grid[i][7], grid[i][8]);
    }*/
	
	memcpy(m_grid, grid, sizeof(m_grid));

    return 1;
}

void * DLX::Malloc(size_t n)
{
    void *p = malloc(n);
    if (p == NULL) 
	{
		/*TRACE(_T("Out of memory in Malloc(%lu)!\n"), (long unsigned)n);*/
		ASSERT(FALSE);
	}
    return p;
}

