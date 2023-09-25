/**********************************************************************
 * Project: Silicon Image-Forest Transform - Modelo Setorial
 * Autor: Fabio Cappabianco
 * Date: 2016
 * 
 * wsantos@20180201 - adaptação para interface axi stream.
**********************************************************************/


/***********************************
 * INCLUDES
************************************/
`include "ifp_set.v"
`include "sc.v"

/***********************************
 * MODULE SIFT Sector Model
************************************/
module axi_sift_sector(
	axis_clk_i,
	axis_rst_i,
	//
	s_axis_tvalid_i,
	s_axis_tready_o,
	s_axis_tlast_i,
	s_axis_tdata_i,
	//
	m_axis_tvalid_o,
	m_axis_tready_i,
	m_axis_tdata_o,
	//	
	type_reg_i,
	mem_ovfl_o,
	run_end_o
);
	parameter IFP_LINE = 16;		// numero setores
	parameter SECTOR_LINE = 3;		// numero linhas
	parameter SECTOR_BITS = 2;		// numero bits do setor
	// control inputs	
	input axis_clk_i;					
	input axis_rst_i;
	// 
	input s_axis_tvalid_i;
	output s_axis_tready_o;
	input s_axis_tlast_i;
	input m_axis_tready_i;
	// register interface
	output m_axis_tvalid_o;
	input [31:0] s_axis_tdata_i;
	output [31:0] m_axis_tdata_o;
	// register and interrupt interface
	input type_reg_i;
	output mem_ovfl_o;
	output run_end_o;

	// Sinais
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
	wire [2*(SECTOR_BITS+6)-1:0] SC_address;
	wire IFPS_finish;
	wire IFPS_direction;
	wire [1:0] SM_state;
	wire [2:0] SM_direction;

/***********************************
 * INSTANCES
 ***********************************/
 
	ifp_set U_ifp_set(
		.axis_clk_i				(axis_clk_i),
		.axis_rst_i				(axis_rst_i),
		.s_axis_tvalid_i		(s_axis_tvalid_i),
		.s_axis_tready_o		(s_axis_tready_o),
		.s_axis_tlast_i			(s_axis_tlast_i),
		.s_axis_tdata_i			(s_axis_tdata_i),
		.m_axis_tvalid_o		(m_axis_tvalid_o),
		.m_axis_tready_i		(m_axis_tready_i),
		.m_axis_tdata_o			(m_axis_tdata_o),
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
		.IFPS_finish			(IFPS_finish),
		.mem_ovfl_o				(mem_ovfl_o)
	);	
		defparam U_ifp_set.IFP_LINE 	= IFP_LINE;
		defparam U_ifp_set.SECTOR_LINE 	= SECTOR_LINE;
		defparam U_ifp_set.SECTOR_BITS 	= SECTOR_BITS;

	sc U_sc(
		.axis_clk_i				(axis_clk_i),
		.axis_rst_i				(axis_rst_i),
		.s_axis_tvalid_i		(s_axis_tvalid_i),
		.s_axis_tlast_i			(s_axis_tlast_i),
		.m_axis_tready_i		(m_axis_tready_i),
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
		.SM_direction_o			(SM_direction),
		.type_reg_i				(type_reg_i),
		.run_end_o				(run_end_o)
	);
		defparam U_sc.SECTOR_LINE = SECTOR_LINE;
		defparam U_sc.SECTOR_BITS = SECTOR_BITS;
   
endmodule
