using System;

namespace Sudoku.Engine
{
	/// <summary>
	/// Summary description for Solver.
	/// </summary>
	public class Solver
	{
		public int[][] A = {new int[10],new int[10],new int[10],new int[10],new int[10],
							   new int[10],new int[10],new int[10],new int[10],new int[10]};
		public int[] Rows = new int[325];
	    public int[] Cols = new int[730];

		public int[][] Row;
		public int[][] Col;
		public int[] Ur = new int[730];
		public int[] Uc = new int[325];
		public int[] C = new int[82];
		public int[] I = new int[82];
		public int[] Node = new int[82];
		int i,j,k,r,c,d,n=729,m=324,x,y,s;
		int min,clues,match;//max

        //just don't forget to set it later ...
        public Solver()
        {
        }

        //overloading the constructor
        public Solver(int[,] inArr)
        {
            SetPuzzle(inArr);
        }

		public Solver(int[][] inArr)
		{

			//
			// TODO: Add constructor logic here
			//

			for(x = 1; x < 10; x++)
				for(y = 1; y < 10; y++)
					A[x][y] = inArr[x-1][y-1];
		}

        //always fails after it's solved one puzzle ... variables probably need
        //to be cleared
        public void SetPuzzle(int[,] inArr)
        {
            for (x = 1; x < 10; x++)
                for (y = 1; y < 10; y++)
                    A[x][y] = inArr[x - 1, y - 1];

        }
        

		public int Solve(int[,] inAr,bool copy)
		{
			int solutions = 0;
			// --------------generate the basic binary exact-cover-matrix,
			// -----that is, not the matrix itself but the rows and columns
			Row = new int[325][];
			Col = new int[730][];
			for(x = 0; x < 325;x++)
				Row[x] = new int[10];
			for(x = 0; x < 730;x++)
				Col[x] = new int[5];
			r=0;
			for(x=1;x<=9;x++)
				for(y=1;y<=9;y++)
					for(s=1;s<=9;s++)
					{
						r++;
						Cols[r]=4;
						Col[r][1]=x*9-9+y;
						Col[r][2]=(3*((x-1)/3)+(y-1)/3)*9+s+81;
						Col[r][3]=x*9-9+s+81*2;
						Col[r][4]=y*9-9+s+81*3;
					}
			for(c=1;c<=m;c++)
				Rows[c]=0;
			for(r=1;r<=n;r++)
				for(c=1;c<=Cols[r];c++)
				{
					x=Col[r][c];
					Rows[x]++;
					Row[x][Rows[x]]=r;
				}

			// --------------do the initial markings required by the given clues----------
			for(i=0;i<=n;i++)
				Ur[i]=0;
			for(i=0;i<=m;i++)
				Uc[i]=0;
			for(x=1;x<=9;x++)
				for(y=1;y<=9;y++)
					if(A[x][y] > 0)
					{
						clues++;
						r=x*81-81+y*9-9+A[x][y];
						for(j=1;j<=Cols[r];j++)
						{
							d=Col[r][j];
							Uc[d]++;
							for(k=1;k<=Rows[d];k++)
							{
								Ur[Row[d][k]]++;
							}
						}
					}

			// -------------backtrack through all subsets of the rows-----------------
			i=0;
			m2://------next level. Compute the next entry------------------
				i++;I[i]=0;
			// ------find the column c=C[i] with fewest matching rows, if empty column
			// is found, then backtrack
			min=n+1;
			for(c=1;c<=m;c++)
			{
				if(Uc[c]==0)
				{
					match=0;for(r=1;r<=Rows[c];r++)if(Ur[Row[c][r]]==0)match++;
					if(match<min){min=match;C[i]=c;}}
				}
			if(min==0 || min>n)goto m4;
			m3://--------walk through all unmarked rows r matching column c=C[i]
			c=C[i];I[i]++;if(I[i]>Rows[c])goto m4;
			r=Row[c][I[i]];if(Ur[r]>0)goto m3;
			x=(r-1)/81+1;
			y=((r-1)%81)/9+1;
			s=(r-1)%9+1;
			A[x][y]=s;
			if(clues+i==81) 
			{ 
				solutions++;
				if(solutions >= 2)
					goto m5;
			}


			// ----delete all columns which match row r and all rows which match 
			//any of these columns
			for(j=1;j<=Cols[r];j++)
			{
				d=Col[r][j];Uc[d]++;
				for(k=1;k<=Rows[d];k++)Ur[Row[d][k]]++;}
			// -------entry was made, matrix was updated, goto the next level---------
			Node[i]++;goto m2;
			m4:// ----backtrack. Goto previous level, take back the last move
				// restore the data as they were before that move and make the next
				// available move at that level
				i--;c=C[i];r=Row[c][I[i]];
			for(j=1;j<=Cols[r];j++)
			{
				d=Col[r][j];Uc[d]--;
				for(k=1;k<=Rows[d];k++){Ur[Row[d][k]]--;}
				}
			if(i>0)goto m3;
			m5:	if(copy) 
			{
				for(int xx = 0;xx < 9;xx++)
					for(int yy = 0; yy < 9; yy++)
						inAr[xx,yy] = A[xx+1][yy+1];
			}
			return(solutions);
		}
	}
}
