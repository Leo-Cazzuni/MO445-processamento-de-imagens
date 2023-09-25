/**************************************************************************************************
	Autor:	Fabio Cappabianco
	@2016-Versao melhorada para tentativa de retomada no projeto. Incompleto!!!
	
	Willian Santos 		
	@20170808-Modificacoes inicais para inicio das simulações
	
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
`include "../../src/axi_sift_sector/axi_sift_sector.v"

/***********************************/
// MODULE TB SIFT sector core
/***********************************/
module axi_sift_sector_tb;

	// Parameter declarations
	parameter TIPO = 3'b011;
	parameter pf = 0;
	// Definem tamanho e como será trarada imagem
	parameter IFP_LINE = 3;		// numero de IFP por linha
	
	parameter SECTOR_LINE = 3;	// Número de pixels da imagem por linha = IFP_LINE*SEXTOR_LINE.
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

	parameter AVL_DATA_SIZE = (IFP_LINE*IFP_LINE < 32) ? IFP_LINE*IFP_LINE : 32;
	parameter IFP_DATA_PER_SECTOR = (IFP_LINE*IFP_LINE) / 32+1;
	parameter nb = 8; 			// 8 é mais usado
	parameter DATA_SIZE = 38;	// Tamanho do dado dentro da IFP. Inclui custo, brilho, rótulo, predecessor.
		// Signals, wires, registers
	integer file;
	reg axis_clk_i = 0;
	reg axis_rst_i = 0;

	reg s_axis_tvalid_i = 0;
	wire s_axis_tready_o;
	reg s_axis_tlast_i = 0;
	reg	[31:0] s_axis_tdata_i;

	wire m_axis_tvalid_o;
	reg m_axis_tready_i = 0;
	wire[31:0] m_axis_tdata_o;

	reg type_reg_i = 0;
		// Input and output file variables
	reg	[AVL_DATA_SIZE-1:0] input_buffer_s[IFP_DATA_PER_SECTOR*SECTOR_LINE*SECTOR_LINE*DATA_SIZE-1 : 0 ];
	reg	[AVL_DATA_SIZE-1:0] result[IFP_DATA_PER_SECTOR*SECTOR_LINE*SECTOR_LINE*(DATA_SIZE-10)-1 : 0 ];
	reg [AVL_DATA_SIZE-1:0] actual_data;
	integer i, word_idx_s;
	integer j;
	integer k;
	integer sector_in, sector_out, ifp_in, ifp_out;
	reg [31:0] fim = 0;
	
	reg is_writing_s;
	reg is_running_s;
	reg is_stoping_s;
	reg is_reading_s;
	reg test_if_s;

/*********************************
* Assignemnts 
*********************************/

/*********************************
*DUT-Design Under Test
*********************************/
	axi_sift_sector axi_sift_sector_u (
		.axis_clk_i			(axis_clk_i),
		.axis_rst_i			(axis_rst_i),
		//
		.s_axis_tvalid_i	(s_axis_tvalid_i),
		.s_axis_tready_o	(s_axis_tready_o),
		.s_axis_tlast_i		(s_axis_tlast_i),
		.s_axis_tdata_i		(s_axis_tdata_i),
		//
		.m_axis_tvalid_o	(m_axis_tvalid_o),
		.m_axis_tready_i	(m_axis_tready_i),
		.m_axis_tdata_o		(m_axis_tdata_o),
		//
		.type_reg_i			(type_reg_i),
		.mem_ovfl_o			(mem_ovfl_o),
		.run_end_o			(run_end_o)
	);
		defparam axi_sift_sector_u.IFP_LINE 	= IFP_LINE;
		defparam axi_sift_sector_u.SECTOR_LINE = SECTOR_LINE;
		defparam axi_sift_sector_u.SECTOR_BITS = SECTOR_BITS;

	
/*********************************
* TB BEGIN
*********************************/
	initial begin
		file = $fopen("result.out","w");
		$dumpfile ("axi_sift_sector_tb.vcd");
		$dumpvars (4, axi_sift_sector_tb);
		#25
		axis_clk_i = 1;
		$readmemh("entradas/intern_extern_3x3_9x9.in", input_buffer_s);
		axis_rst_i = 1'b1;
		#25
		axis_clk_i = ~axis_clk_i;
		#25
		axis_clk_i = ~axis_clk_i;
		axis_rst_i = 1'b0;

		/* Writing */
		is_writing_s = 1'b1;
		$display("write:");
		for (i = 0; i < SECTOR_LINE*SECTOR_LINE*DATA_SIZE; i = i+1) begin
			for (j = 0; j < (IFP_LINE*IFP_LINE)/32; j = j+1) begin
				actual_data = input_buffer_s[i*((IFP_LINE*IFP_LINE-1) / 32+1)+j];
				for (word_idx_s = 0; word_idx_s < 32; word_idx_s = word_idx_s+1) begin
					while (s_axis_tready_o != 1) begin
						#25
						axis_clk_i = ~axis_clk_i;
						#25
						axis_clk_i = ~axis_clk_i;
					end
					s_axis_tvalid_i = 1;
					s_axis_tdata_i[word_idx_s] = actual_data[word_idx_s];
				end
				#25
				axis_clk_i = ~axis_clk_i;
				#25
				axis_clk_i = ~axis_clk_i;
			end
			if (i == 341) begin
				s_axis_tlast_i = 1;
			end
			if (IFP_LINE*IFP_LINE % 32 != 0) begin
				actual_data = input_buffer_s[i*((IFP_LINE*IFP_LINE-1) / 32+1)+j];
				for (word_idx_s = 0; word_idx_s < IFP_LINE*IFP_LINE % 32; word_idx_s = word_idx_s+1) begin
					while (s_axis_tready_o != 1) begin
						#25
						axis_clk_i = ~axis_clk_i;
						#25
						axis_clk_i = ~axis_clk_i;
					end
					s_axis_tvalid_i = 1;
					s_axis_tdata_i[word_idx_s] = actual_data[word_idx_s];
				end
				for (word_idx_s = IFP_LINE*IFP_LINE % 32; word_idx_s < 32; word_idx_s = word_idx_s+1) begin
					while (s_axis_tready_o != 1) begin
						#25
						axis_clk_i = ~axis_clk_i;
						#25
						axis_clk_i = ~axis_clk_i;
					end
					s_axis_tvalid_i = 1;
					s_axis_tdata_i[word_idx_s] = 1'b0;
				end
				#25
				axis_clk_i = ~axis_clk_i;
				#25
				axis_clk_i = ~axis_clk_i;
			end
		end
		is_writing_s = 1'b0;	
		s_axis_tvalid_i = 0;
		s_axis_tlast_i = 0;

		/* Running */
		is_running_s = 1'b1;
		$display("Prepare run:");
		#25
		axis_clk_i = ~axis_clk_i;
		#25
		axis_clk_i = ~axis_clk_i;
		$display("Start run:");
		while (run_end_o != 1'b1) begin
			#25
			axis_clk_i = ~axis_clk_i;
			#25
			axis_clk_i = ~axis_clk_i;
		end
		is_running_s = 1'b0;
		
		/* Stoping */
		is_stoping_s = 1'b1;
		$display("Stoping:");
		#25
		axis_clk_i = ~axis_clk_i;
		#25
		axis_clk_i = ~axis_clk_i;
		#25
		axis_clk_i = ~axis_clk_i;
		#25
		axis_clk_i = ~axis_clk_i;
		#25
		axis_clk_i = ~axis_clk_i;
		#25
		axis_clk_i = ~axis_clk_i;
		is_stoping_s = 1'b0;

		/* Reading */
		is_reading_s = 1'b1;
		$display("Start Read:");
		for (i = 0; i < SECTOR_LINE*SECTOR_LINE*(DATA_SIZE-10); i = i+1) begin
			#25
			axis_clk_i = ~axis_clk_i;
			#25
			axis_clk_i = ~axis_clk_i;
			m_axis_tready_i = 1'b1;
			#25
			axis_clk_i = ~axis_clk_i;
			#25
			axis_clk_i = ~axis_clk_i;
//			for (j = 0; j < (IFP_LINE*IFP_LINE) / 32; j = j+1) begin
//				#25
//				axis_clk_i = ~axis_clk_i;
//				#25
//				axis_clk_i = ~axis_clk_i;
//				for (word_idx_s = 0; word_idx_s < 32; word_idx_s = word_idx_s+1) begin
//					$fwrite(file, "%d ", m_axis_tdata_o[word_idx_s ]);
//					result[j*32+i*IFP_LINE*IFP_LINE+word_idx_s] = m_axis_tdata_o[word_idx_s];
//				end
//				#25
//				axis_clk_i = ~axis_clk_i;
//				#25
//				axis_clk_i = ~axis_clk_i;
//			end
			if ((IFP_LINE*IFP_LINE) % 32 != 0) begin
				test_if_s = 1'b1;
				#25
				axis_clk_i = ~axis_clk_i;
				#25
				axis_clk_i = ~axis_clk_i;
				for (word_idx_s = 0; word_idx_s < (IFP_LINE*IFP_LINE) % 32; word_idx_s = word_idx_s+1) begin
					$fwrite(file, "%d ", m_axis_tdata_o[word_idx_s ]);
					result[j*32+i*IFP_LINE*IFP_LINE+word_idx_s ] = m_axis_tdata_o[word_idx_s ];
				end
				#25
				axis_clk_i = ~axis_clk_i;
				#25
				axis_clk_i = ~axis_clk_i;
				$fdisplay(file, "");
			end
			test_if_s = 1'b0;
		end
		#25
		axis_clk_i = ~axis_clk_i;
		m_axis_tready_i = 1'b0;
		#25
		axis_clk_i = ~axis_clk_i;
		is_reading_s = 1'b0;
		
		$display("Fim");
		$fclose(file);
	end

endmodule
