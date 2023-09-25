// Project: Silicon Image-Forest Transform - Modelo Setorial
// Autor: Fabio Cappabianco
// Date: 2016

/***********************************/
// INCLUDES
/***********************************/
`include "ifp_set.v"
`include "sc.v"

/***********************************/
// MODULE SIFT Sector Model
/***********************************/
module sift_sector_core(
	clk_i,
	reset_i,
	type_reg_i,
	cmd_reg_i,
	chipselect_i,
	write_i,
	read_i,
	address_i,
	writedata_i,
	readdata_o
);
	parameter IFP_LINE = 16;		// numero setores
	parameter SECTOR_LINE = 3;		// numero linhas
	parameter SECTOR_BITS = 2;		// numero bits do setor
	input clk_i;					// control inputs
	input reset_i;
	// register interface
	input type_reg_i;
	input [3:0] cmd_reg_i;
	input chipselect_i;
	input write_i;
	input read_i;
	input address_i;
	input [ 31 : 0 ] writedata_i;	// data input
	output [ 31 : 0 ] readdata_o;	// data output

	// Sinais
	wire [31:0] sc_data_out_s;
	wire [31:0] ifp_set_data_out_s;
	wire SC_read;
	wire SC_send;
	wire SC_write;
	wire SC_receive;
	wire SC_run;
	wire [1:0] SC_pathfunction;
	wire SC_neighborhood;
	wire SC_neighbor_address;
	wire SC_data_type;
	wire SC_carry_in;
	wire [2*(SECTOR_BITS + 6 ) - 1 : 0 ] SC_address;
	wire IFPS_finish;
	wire IFPS_direction;
	wire [1:0] SM_state;
	wire [2:0] SM_direction;

	assign readdata_o = ( address_i == 0 ) ? sc_data_out_s : ifp_set_data_out_s;

	
/***********************************
 * INSTANCES
 ***********************************/
 
	// instance of ifp_set component
	ifp_set U_ifp_set(
		.clock					(clk_i),
		.AVL_chipselect			(chipselect_i),
		.AVL_write				(write_i),
		.AVL_address			(address_i),
		.AVL_writedata			(writedata_i),
		.AVL_readdata			(ifp_set_data_out_s),
		.SC_read				(SC_read),
		.SC_send				(SC_send),
		.SC_receive				(SC_receive),
		.SC_write				(SC_write),
		.SC_run					(SC_run),
		.SC_address				(SC_address),
		.SC_pathfunction		(SC_pathfunction),
		.SC_neighborhood		(SC_neighborhood),
		.SC_neighbor_address	(SC_neighbor_address),
		.SC_data_type			(SC_data_type),
		.SC_carry_in			(SC_carry_in),
		.SM_state				(SM_state),
		.SM_direction			(SM_direction),
		.IFPS_direction			(IFPS_direction),
		.IFPS_finish			(IFPS_finish)
	);	
		defparam U_ifp_set.IFP_LINE 	= IFP_LINE;
		defparam U_ifp_set.SECTOR_LINE 	= SECTOR_LINE;
		defparam U_ifp_set.SECTOR_BITS 	= SECTOR_BITS;

	// instance of sc component
	sc U_sc(
		.clock_i				(clk_i),
		.reset_i				(reset_i),
		.type_reg_i				(type_reg_i),
		.cmd_reg_i				(cmd_reg_i),
		.AVL_write_i			(write_i),
		.AVL_address_i			(address_i),
		.AVL_writedata_i		(writedata_i),
		.AVL_readdata_o			(sc_data_out_s),
		.IFPS_finish_i			(IFPS_finish),
		.IFPS_direction_o		(IFPS_direction),
		.SC_read_o				(SC_read),
		.SC_send_o				(SC_send),
		.SC_receive_o			(SC_receive),
		.SC_write_o				(SC_write),
		.SC_run_o				(SC_run),
		.SC_address_o			(SC_address),
		.SC_pathfunction_o		(SC_pathfunction),
		.SC_data_type_o			(SC_data_type),
		.SC_neighborhood_o		(SC_neighborhood),
		.SC_neighbor_address_o	(SC_neighbor_address),
		.SC_carry_in_o			(SC_carry_in),
		.SM_state_o				(SM_state),
		.SM_direction_o			(SM_direction)
	);
		defparam U_sc.SECTOR_LINE = SECTOR_LINE;
		defparam U_sc.SECTOR_BITS = SECTOR_BITS;
   
endmodule
