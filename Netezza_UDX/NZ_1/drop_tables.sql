CREATE OR REPLACE PROCEDURE drop_tables(INTEGER, INTEGER) RETURNS BOOL
LANGUAGE NZPLSQL AS
BEGIN_PROC
        DECLARE
		rows ALIAS FOR $1;
                cols ALIAS FOR $2;
		drop_table_statement VARCHAR ;

	BEGIN
		drop_table_statement := 'DROP TABLE NZ_INPUT_TABLE_' || rows || '_' || cols;
                execute immediate drop_table_statement;
		drop_table_statement := 'DROP TABLE NZ_INP_MEAN_' || rows || '_' || cols;
		execute immediate drop_table_statement;
		drop_table_statement := 'DROP TABLE NZ_INPMEAN_DIFF_' || rows || '_' || cols;
                execute immediate drop_table_statement;
		drop_table_statement := 'DROP TABLE NZ_OBS_SDEV_' || rows || '_' || cols;
                execute immediate drop_table_statement;
		drop_table_statement := 'DROP TABLE NZ_DIFF_SDEV_' || rows || '_' || cols;
                execute immediate drop_table_statement;
		 drop_table_statement := 'DROP TABLE NZ_OBS_CORR_' || rows || '_' || cols;
                execute immediate drop_table_statement;
		 drop_table_statement := 'DROP TABLE NZA_CORR_' || rows || '_' || cols;
                execute immediate drop_table_statement;
		
	END;
END_PROC;
