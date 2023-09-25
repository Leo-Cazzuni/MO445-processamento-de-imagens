/**************************************************************************************************
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
***************************************************************************************************/


/***********************************/
// INCLUDES
/***********************************/
`include "../../src/sift_ip/sift_sector_core.v"

/***********************************/
// MODULE TB SIFT sector core
/***********************************/
module sift_sector_core_tb;

// Parameter declarations
	parameter TIPO = 3'b011;
	parameter pf = 0;
	
// Definem tamanho e como será trarada imagem
	parameter IFP_LINE = 3;		// numero de IFP por linha
	
	parameter SECTOR_LINE = 3;	// Número de pixels da imagem por linha = IFP_LINE * SEXTOR_LINE.
								// A imagem é dividida em setores e tem o tamanho da matriz de IFPs,
								// neste caso 3x3. SECTOR_LINE funciona da mesma maneira que IFP_LINE.
								// Se SECTOR_LINE = 3 então temos 9 setores em uma matriz de 3x3
								// setores. Cada setor possui 9 pixels em uma matriz de 3x3. Ao todo,
								// temos portanto 81 pixels em uma matriz de 9x9.
								
	parameter SECTOR_BITS = 2;	// SECTOR_BITS indica quantos bits de endereço são necessários para
								// endereçar os setores existentes. Como temos 9 setores em uma
								// matriz 3x3, então precisamos de 2 bits de endereço para linha e
								// 2 bits de endereço para coluna. Se não me engano, o endereço é
								// multiplexado, mas não tenho absoluta certeza.

	parameter AVL_DATA_SIZE = ( IFP_LINE * IFP_LINE < 32 ) ? IFP_LINE * IFP_LINE : 32;
	parameter IFP_DATA_PER_SECTOR = ( IFP_LINE * IFP_LINE ) / 32 + 1;
	
	parameter nb = 8; 			// 8 é mais usado
	parameter DATA_SIZE = 38;	// Tamanho do dado dentro da IFP. Inclui custo, brilho, rótulo, predecessor.
	
// Signals, wires, registers
	integer file;
	reg clk_i;
	reg reset_i;
	reg [3:0] cmd_reg_i;
	reg type_reg_i;
	reg chipselect_i;
	reg write_i;
	reg read_i;
	reg address_i;
	reg	[31:0] writedata_i;
	wire[31:0] readdata_o;
	
// Input and output file variables
	reg	[AVL_DATA_SIZE-1:0] input_buffer_s[ IFP_DATA_PER_SECTOR * SECTOR_LINE * SECTOR_LINE * DATA_SIZE - 1 : 0 ];
	reg	[AVL_DATA_SIZE-1:0] result[ IFP_DATA_PER_SECTOR * SECTOR_LINE * SECTOR_LINE * ( DATA_SIZE - 10 ) - 1 : 0 ];
	reg [AVL_DATA_SIZE-1:0] actual_data;
	integer i, word_idx_s;
	integer j;
	integer k;
	integer sector_in, sector_out, ifp_in, ifp_out;
	reg [31:0] fim;
	reg is_writing_s;
	reg is_running_s;
	reg is_stoping_s;
	reg is_reading_s;
	reg test_if_s;
	
/*********************************/
// DUT - Design Under Test
/*********************************/
	sift_sector_core sift_sector_core_u (
		.clk_i			(clk_i),
		.reset_i		(reset_i),
		.type_reg_i		(type_reg_i),
		.cmd_reg_i		(cmd_reg_i),
		.chipselect_i	(chipselect_i),
		.write_i		(write_i),
		.read_i			(read_i),
		.address_i		(address_i),
		.writedata_i	(writedata_i),
		.readdata_o		(readdata_o)
	);
	
// Passagem de parametros para as entidades inferiores
	defparam sift_sector_core_u.IFP_LINE 	= IFP_LINE;
	defparam sift_sector_core_u.SECTOR_LINE = SECTOR_LINE;
	defparam sift_sector_core_u.SECTOR_BITS = SECTOR_BITS;

	
/*********************************/
// TB BEGIN
/*********************************/
	initial begin
		file = $fopen("result.out","w");
		$dumpfile ("sift_sector_core_tb.vcd");
		$dumpvars (4, sift_sector_core_tb);
		type_reg_i = 0;
		cmd_reg_i = 0;
		writedata_i = 0;
		chipselect_i = 1'b0;
		write_i = 1'b0;
		read_i = 1'b0;
		address_i = 1'b0;
		#50
		clk_i = 1;
		$readmemh("entradas/intern_extern_3x3_9x9.in", input_buffer_s);
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

		is_writing_s = 1'b1;	
		$display("write:");
		for (i = 0; i < SECTOR_LINE * SECTOR_LINE * DATA_SIZE; i = i+1 ) begin
			for (j = 0; j < (IFP_LINE * IFP_LINE)/32; j = j+1 ) begin
				actual_data = input_buffer_s[i*((IFP_LINE * IFP_LINE - 1) / 32 + 1) + j];
				#5
				for (word_idx_s = 0; word_idx_s < 32; word_idx_s = word_idx_s + 1) begin
					writedata_i[word_idx_s] = actual_data[word_idx_s];
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
			end
			if (IFP_LINE * IFP_LINE % 32 != 0) begin
				actual_data = input_buffer_s[ i * ((IFP_LINE * IFP_LINE - 1) / 32 + 1) + j];
				#5
				for (word_idx_s = 0; word_idx_s < IFP_LINE * IFP_LINE % 32; word_idx_s = word_idx_s + 1) begin
					writedata_i[word_idx_s] = actual_data[word_idx_s];
				end
				for (word_idx_s = IFP_LINE * IFP_LINE % 32; word_idx_s < 32; word_idx_s = word_idx_s + 1) begin
					writedata_i[word_idx_s] = 1'b0;
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
		is_writing_s = 1'b0;	

		is_running_s = 1'b1;	
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
		cmd_reg_i = 4'b0011;
		fim = 32'h0;
		$display("Start run:");
		while ( fim != 7 ) begin
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
		is_running_s = 1'b0;	

		/* Stoping */
		is_stoping_s = 1'b1;
		$display("Prepare read_i:");
		#5
		writedata_i = 2 + nb + pf;	// STOP + NB + PF
		chipselect_i = 1'b1;
		read_i = 1'b0;
		write_i = 1'b1;
		address_i = 1'b0;
		#20
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		cmd_reg_i = 4'b0010;
		#25
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		is_stoping_s = 1'b0;

		/* Reading */
		is_reading_s = 1'b1;
		$display("Start read_i:");
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
			chipselect_i = 1'b0;
			write_i = 1'b0;
			#20
			clk_i = ~clk_i;
			#25
			clk_i = ~clk_i;
			#5
//			for ( j = 0; j < ( IFP_LINE * IFP_LINE ) / 32; j = j + 1 ) begin
//				#5
//				chipselect_i = 1'b1;
//				write_i = 1'b0;
//				read_i = 1'b1;
//				address_i = 1'b1;
//				#20
//				clk_i = ~clk_i;
//				#25
//				clk_i = ~clk_i;
//				for ( word_idx_s = 0; word_idx_s < 32; word_idx_s = word_idx_s + 1 ) begin
//					$fwrite(file, "%d ", readdata_o[ word_idx_s ]);
//					result[ j * 32 + i * IFP_LINE * IFP_LINE + word_idx_s ] = readdata_o[ word_idx_s ];
//				end
//				#5
//				chipselect_i = 1'b1;
//				write_i = 1'b0;
//				read_i = 1'b1;
//				address_i = 1'b0;
//				#20
//				clk_i = ~clk_i;
//				#25
//				clk_i = ~clk_i;
//			end
			if ( ( IFP_LINE * IFP_LINE ) % 32 != 0 ) begin
				test_if_s = 1'b1;
				#5
				chipselect_i = 1'b1;
				write_i = 1'b0;
				read_i = 1'b1;
				address_i = 1'b1;
				#20
				clk_i = ~clk_i;
				#25
				clk_i = ~clk_i;
				for ( word_idx_s = 0; word_idx_s < ( IFP_LINE * IFP_LINE ) % 32; word_idx_s = word_idx_s + 1 ) begin
					$fwrite(file, "%d ", readdata_o[ word_idx_s ]);
					result[ j * 32 + i * IFP_LINE * IFP_LINE + word_idx_s ] = readdata_o[ word_idx_s ];
				end
				#5
				address_i = 1'b0;
				#20
				clk_i = ~clk_i;
				#25
				clk_i = ~clk_i;
				$fdisplay(file, "");
			end
			test_if_s = 1'b0;
		end
		#5
		chipselect_i = 1'b0;
		write_i = 1'b0;
		read_i = 1'b0;
		#20
		clk_i = ~clk_i;
		#25
		clk_i = ~clk_i;
		is_reading_s = 1'b0;	
		
		$display("Fim");
		$fclose(file);
	end

endmodule
