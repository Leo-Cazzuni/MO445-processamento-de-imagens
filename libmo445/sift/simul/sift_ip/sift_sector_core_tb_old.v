/*
	Autor:	Fabio Cappabianco
	@2016 - Versao melhorada para tentativa de retomada no projeto. Incompleto!!!
	
	Willian Santos 		
	@20170808 - Modificacoes inicais para inicio das simulações
	
	Testbench mais recente da sift setorial em hardware 

	Necessário rodar programa ifp_set.c para gerar o hardware do tamanho correto.
	
	3 Tipos de processamento:
		| cod	| tipo 				| 
		| 0		| suprec			|
		| 1		| ctrack, geodesic	|

	PF relacionado ao Topo acima:
		| cod 	| pf		|
		| 0		| suprec	|
		| 16	| lsuprec	|
		| 32	| geo		|
		| 48	| ctrack	|
		
	NB = vizinhança
		| cod	| vizinhanca|
		| 0		| nb4		|
		| 8		| nb8		|

*/
/***********************************/
// INCLUDES
/***********************************/
`include "../../src/sift_ip/sift_sector_core.v"

/***********************************/
// MODULE TB SIFT sector core
/***********************************/
module sift_sector_core_tb_old;

// Parameter declarations
	parameter TIPO = 3'b011;
	parameter pf = 0;
	
// Definem tamanho e como será trarada imagem
	parameter IFP_LINE = 3;					// hw 16x16 operadores
	parameter SECTOR_LINE = 3;				//	combinado com ifp_line dá dimensão do hw.
	parameter SECTOR_BITS = 2;				// log base 2 do sector bits
	parameter AVL_DATA_SIZE = ( IFP_LINE * IFP_LINE < 32 ) ? IFP_LINE * IFP_LINE : 32;
	parameter IFP_DATA_PER_SECTOR = ( IFP_LINE * IFP_LINE ) / 32 + 1;
	
	parameter nb = 8; 						// 8 é mais usado
	parameter DATA_SIZE = 38;				// numero de colunas no arquivo.
	
// Signals, wires, registers
	integer file;
	reg clk_i;
	reg reset_i;
	reg chipselect_i;
	reg write_i;
	reg read_i;
	reg address_i;
	reg	[31:0] writedata_i;
	wire[31:0] readdata_o;
	
// Input and output file variables
	reg	[AVL_DATA_SIZE-1:0] Prog[ IFP_DATA_PER_SECTOR * SECTOR_LINE * SECTOR_LINE * DATA_SIZE - 1 : 0 ];
	reg	[AVL_DATA_SIZE-1:0] result[ IFP_DATA_PER_SECTOR * SECTOR_LINE * SECTOR_LINE * ( DATA_SIZE - 10 ) - 1 : 0 ];
	
	reg [AVL_DATA_SIZE-1:0] actual_data;
	integer i, p;
	integer j;
	integer k;
	integer word;
	integer sector_in, sector_out, ifp_in, ifp_out;
	reg [31:0] fim;

	
/*********************************/
// DUT - Design Under Test
/*********************************/
	sift_sector_core sift_sector_core_u (
		.clk_i			(clk_i),
		.reset_i		(reset_i),
		.chipselect_i	(chipselect_i),
		.write_i		(write_i),
		.read_i			(read_i),
		.address_i		(address_i),
		.writedata_i	(writedata_i),
		.readdata_o		(readdata_o)
	);
	
// Passagem de parametros para as entidades inferiores
	defparam sift_sector_core_u.IFP_LINE = IFP_LINE;
	defparam sift_sector_core_u.SECTOR_LINE = SECTOR_LINE;
	defparam sift_sector_core_u.SECTOR_BITS = SECTOR_BITS;

	
/*********************************/
// TB BEGIN
/*********************************/
	initial begin
		file = $fopen("result.out","w");
		$dumpfile ("sift_sector_core_tb_old.vcd");
		$dumpvars (4, sift_sector_core_tb_old);
		writedata_i = 0;
		chipselect_i = 1'b0;
		write_i = 1'b0;
		read_i = 1'b0;
		address_i = 1'b0;
		#50
		clk_i = 1;
		$readmemh("entradas/intern_extern_3x3_9x9.in", Prog);
		reset_i = 1'b1;
		#25
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		reset_i = 1'b0;
		#25
		clk_i = ~clk_i;
		#5
		writedata_i = TIPO;
		chipselect_i = 1'b1;
		write_i = 1'b1;
		address_i = 1'b0;
		#20
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		// write
		$display("write:");
		for ( i = 0; i < SECTOR_LINE * SECTOR_LINE * DATA_SIZE; i = i + 1 ) begin
			for ( j = 0; j < ( IFP_LINE * IFP_LINE ) / 32; j = j + 1 ) begin
				actual_data = Prog[ i * ( ( IFP_LINE * IFP_LINE - 1 ) / 32 + 1 ) + j ];
				#5
				for ( p = 0; p < 32; p = p + 1 ) begin
					writedata_i[ p ] = actual_data[ p ];
				end
				chipselect_i = 1'b1;
				write_i = 1'b1;
				read_i = 1'b0;
				address_i = 1'b1;
				#20
				clk_i = ~clk_i;
				#25
				clk_i = ~clk_i;
				#5
				chipselect_i = 1'b1;
				write_i = 1'b0;
				read_i = 1'b1;
				address_i = 1'b0;
				#20
				clk_i = ~clk_i;
				#25
				clk_i = ~clk_i;
				fim = readdata_o;
				$write("write_i [%d, %d]: fim = %x, data = %x\n",i,j,fim,writedata_i);
			end
			if ( IFP_LINE * IFP_LINE % 32 != 0 ) begin
				actual_data = Prog[ i * ( ( IFP_LINE * IFP_LINE - 1 ) / 32 + 1 ) + j ];
				#5
				for ( p = 0; p < IFP_LINE * IFP_LINE % 32; p = p + 1 ) begin
					writedata_i[ p ] = actual_data[ p ];
				end
				//
				for ( p = IFP_LINE * IFP_LINE % 32; p < 32; p = p + 1 ) begin
					writedata_i[ p ] = 1'b0;
				end
				chipselect_i = 1'b1;
				write_i = 1'b1;
				read_i = 1'b0;
				address_i = 1'b1;
				#20
				clk_i = ~clk_i;
				#25
				clk_i = ~clk_i;
				#5
				chipselect_i = 1'b1;
				write_i = 1'b0;
				read_i = 1'b1;
				address_i = 1'b0;
				#20
				clk_i = ~clk_i;
				#25
				clk_i = ~clk_i;
				fim = readdata_o;
				$write("write_i [%d, %d]: fim = %x, data = %x\n",i,j,fim,writedata_i);
			end
			#5
			writedata_i = 3'b001;
			chipselect_i = 1'b1;
			write_i = 1'b1;
			address_i = 1'b0;
			#20
			clk_i = ~clk_i;
			#25
			clk_i = ~clk_i;
		end
		// run
		$display("Prepare run:");
		#5
		writedata_i = 6 + nb + pf; // START + NB + PF
		chipselect_i = 1'b1;
		read_i = 1'b0;
		write_i = 1'b1;
		address_i = 1'b0;
		#20
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		fim = 32'h0;
		$display("Start run:");
		while ( fim != 7 ) begin
			$display("begin run clk_i = %d", clk_i);
			#5
			chipselect_i = 1'b1;
			write_i = 1'b0;
			read_i = 1'b1;
			address_i = 1'b0;
			#20
			clk_i = ~clk_i;
			//$display("clk_i = 0");
			#25
			clk_i = ~clk_i;
			fim = readdata_o;
			$display("end run: fim = %x ",fim);
		end
		// read_i
		$display("Prepare read_i:");
		#5
		writedata_i = 2 + nb + pf;// STOP + NB + PF
		chipselect_i = 1'b1;
		read_i = 1'b0;
		write_i = 1'b1;
		address_i = 1'b0;
		#20
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		$display("Start read_i:");
		$fdisplay(file, "%d %d", IFP_LINE, SECTOR_LINE);
		for ( i = 0; i < SECTOR_LINE * SECTOR_LINE * ( DATA_SIZE - 10 ); i = i + 1 ) begin
			#5
			writedata_i = 3'b000;
			chipselect_i = 1'b1;
			read_i = 1'b0;
			write_i = 1'b1;
			address_i = 1'b0;
			#20
			clk_i = ~clk_i;
			#25
			clk_i = ~clk_i;
			#5
			writedata_i = 3'b000;
			chipselect_i = 1'b0;
			read_i = 1'b0;
			write_i = 1'b0;
			address_i = 1'b0;
			#20
			clk_i = ~clk_i;
			#25
			clk_i = ~clk_i;
			#5
			for ( j = 0; j < ( IFP_LINE * IFP_LINE ) / 32; j = j + 1 ) begin
				#5
				chipselect_i = 1'b1;
				write_i = 1'b0;
				read_i = 1'b1;
				address_i = 1'b1;
				#20
				clk_i = ~clk_i;
				#25
				clk_i = ~clk_i;
				for ( p = 0; p < 32; p = p + 1 ) begin
					$write("%d ", readdata_o[ p ]);
					$fwrite(file, "%d ", readdata_o[ p ]);
					result[ j * 32 + i * IFP_LINE * IFP_LINE + p ] = readdata_o[ p ];
				end
				#5
				chipselect_i = 1'b1;
				write_i = 1'b0;
				read_i = 1'b1;
				address_i = 1'b0;
				#20
				clk_i = ~clk_i;
				#25
				clk_i = ~clk_i;
				fim = readdata_o;
			end
			if ( ( IFP_LINE * IFP_LINE ) % 32 != 0 ) begin
				#5
				chipselect_i = 1'b1;
				write_i = 1'b0;
				read_i = 1'b1;
				address_i = 1'b1;
				#20
				clk_i = ~clk_i;
				#25
				clk_i = ~clk_i;
				//$write("dado[%d][%d] = %x ", i, j, readdata_o);
				for ( p = 0; p < ( IFP_LINE * IFP_LINE ) % 32; p = p + 1 ) begin
					$write("%d ", readdata_o[ p ]);
					$fwrite(file, "%d ", readdata_o[ p ]);
					result[ j * 32 + i * IFP_LINE * IFP_LINE + p ] = readdata_o[ p ];
				end
				#5
				chipselect_i = 1'b1;
				write_i = 1'b0;
				read_i = 1'b1;
				address_i = 1'b0;
				#20
				clk_i = ~clk_i;
				#25
				clk_i = ~clk_i;
				fim = readdata_o;
				$fdisplay(file, "");
				$display("");
			end
		end
		#5
		chipselect_i = 1'b0;
		write_i = 1'b0;
		read_i = 1'b0;
		#20
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		
		$display("Fim");
		$fclose(file);
		//$stop;
	end

endmodule
