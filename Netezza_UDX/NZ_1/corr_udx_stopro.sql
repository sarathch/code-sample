CREATE OR REPLACE PROCEDURE netezza_correlation_matrix(INTEGER, INTEGER) RETURNS INT4 
LANGUAGE NZPLSQL AS 
BEGIN_PROC 
	DECLARE 
		i INTEGER; 
		j INTEGER; 
		start_time TIME;
		end_time TIME;
		time_taken TIME;
		rows ALIAS FOR $1; 
		cols ALIAS FOR $2; 
		create_table_statement VARCHAR ; 
		insert_table_statement VARCHAR ;
		select_table_statement VARCHAR ;
		ctas_mean_statement VARCHAR ; 	
		ctas_diff_statement VARCHAR ;		
		ctas_sDev_statement VARCHAR ;
		ctas_diffdev_statement VARCHAR ;
		ctas_calcorr_statement VARCHAR ;
		create_time_table VARCHAR ;
		nza_corr_matrix VARCHAR ;
			
		BEGIN
			--********************************************************************************************************************
			--CREATE TABLE BLOCK 
			--********************************************************************************************************************
			create_table_statement := 'CREATE TABLE NZ_INPUT_TABLE_' || rows || '_' || cols || ' (';
			 --comma issue workaround 
			j := 0; 
			create_table_statement := create_table_statement || 'col' || j || ' DOUBLE';
			FOR j IN 1 .. cols-1 
				LOOP 
					create_table_statement := create_table_statement || ', col' || j || ' DOUBLE';
				 END LOOP; 
			create_table_statement := create_table_statement || ');'; 
			execute immediate create_table_statement; 
			--RAISE NOTICE 'Create table statement is %', create_table_statement;

			--**********************************************************************************************************************
			--INSERT TABLE BLOCK 
			--**********************************************************************************************************************
			insert_table_statement := 'INSERT INTO NZ_INPUT_TABLE_' || rows || '_' || cols || ' VALUES (';
			FOR i IN 0 .. rows-1
				LOOP
					j:=0;
					insert_table_statement := insert_table_statement || trunc( random()*(100-5), 2);
					FOR j IN 1 .. cols-1
		                                LOOP
                		                        insert_table_statement := insert_table_statement || ', ' || trunc( random()*(100-5),2);
                                		 END LOOP;
					insert_table_statement := insert_table_statement || ');';
		                        execute immediate insert_table_statement;
					--RAISE NOTICE 'Insert table statement is %', insert_table_statement;
					insert_table_statement := 'INSERT INTO NZ_INPUT_TABLE_' || rows || '_' || cols || ' VALUES (';
				END LOOP;
			RAISE NOTICE 'Insert Completed';
			
			select current_time INTO start_time;
			
			RAISE NOTICE 'TIME IS %',start_time;
			
			--***********************************************************************************************************************
			-- CAL MEAN OF INPUT BLOCK
			--***********************************************************************************************************************
			ctas_mean_statement := ' CREATE TABLE NZ_INP_MEAN_' || rows || '_' || cols || ' AS SELECT ';
			FOR j IN 0 .. cols-2
                                LOOP
                                        ctas_mean_statement := ctas_mean_statement || 'AVG(col' || j || ') MEAN_' || j || ', ';
                                 END LOOP;
			j := cols-1;
			ctas_mean_statement := ctas_mean_statement || 'AVG(col' || j || ') MEAN_' || j; 
                        ctas_mean_statement := ctas_mean_statement || ' FROM NZ_INPUT_TABLE_' || rows || '_' || cols || ';';
			execute immediate ctas_mean_statement;
                        --RAISE NOTICE 'ctas table statement is %', ctas_mean_statement;
			
			--************************************************************************************************************************
			-- CTAS DIFF USING DIFF_ UDTF
			--************************************************************************************************************************
			ctas_diff_statement := ' CREATE TABLE NZ_INPMEAN_DIFF_' || rows || '_' || cols || ' AS SELECT ';
			 j := 0;
                        ctas_diff_statement := ctas_diff_statement || 'id_' || j || '.XDiff DIFF_' || j;
                        FOR j IN 1 .. cols-1
                                LOOP
                                        ctas_diff_statement := ctas_diff_statement || ', id_' || j || '.XDiff DIFF_' || j;
                                 END LOOP;
                        ctas_diff_statement := ctas_diff_statement || ' FROM NZ_INPUT_TABLE_' || rows || '_' || cols || ', NZ_INP_MEAN_' || rows || '_' || cols || '';
			FOR j IN 0 .. cols-1
                                LOOP
                                        ctas_diff_statement := ctas_diff_statement || ', TABLE ( diff_ (col' || j || ',MEAN_' || j || ') ) AS id_' || j;
                                 END LOOP;
			ctas_diff_statement := ctas_diff_statement || ';';
                        execute immediate ctas_diff_statement;
                        --RAISE NOTICE 'ctas table diff statement is %', ctas_diff_statement;
			
			--*************************************************************************************************************************
			-- CTAS SDEV USING caldev UDA
			--*************************************************************************************************************************
			ctas_sDev_statement := ' CREATE TABLE NZ_OBS_SDEV_' || rows || '_' || cols || ' AS SELECT ';
			  j := 0;
                        ctas_sDev_statement := ctas_sDev_statement || 'caldev(DIFF_' || j || ') SDEV_' || j;
                        FOR j IN 1 .. cols-1
                                LOOP
                                        ctas_sDev_statement := ctas_sDev_statement || ', caldev(DIFF_' || j || ') SDEV_' || j;
                                 END LOOP;
                        ctas_sDev_statement := ctas_sDev_statement || ' FROM NZ_INPMEAN_DIFF_' || rows || '_' || cols || ';';
                        execute immediate ctas_sDev_statement;
                        --RAISE NOTICE 'ctas table SDEV statement is %', ctas_sDev_statement;
			
			ctas_diffdev_statement := ' CREATE TABLE NZ_DIFF_SDEV_' || rows || '_' || cols || ' AS SELECT ';
			j := 0;
			ctas_diffdev_statement := ctas_diffdev_statement || 't.DIFF_' || j || ', p.SDEV_' || j;
			FOR j IN 1 .. cols-1
                                LOOP
                                        ctas_diffdev_statement := ctas_diffdev_statement || ', t.DIFF_' || j || ', p.SDEV_' || j;
                                 END LOOP;
                        ctas_diffdev_statement := ctas_diffdev_statement || ' FROM NZ_INPMEAN_DIFF_' || rows || '_' || cols || ' AS t, NZ_OBS_SDEV_' || rows || '_' || cols || ' AS p;';
                        execute immediate ctas_diffdev_statement;
                        --RAISE NOTICE 'ctas table diffdev statement is %', ctas_diffdev_statement;
			
			--****************************************************************************************************************************
			-- CTAS CORRELATION USING calcorr UDA
			--****************************************************************************************************************************
			ctas_calcorr_statement := ' CREATE TABLE NZ_OBS_CORR_' || rows || '_' || cols || ' AS SELECT ';
			FOR i IN 0 .. cols-2
				LOOP
					j := 0;
					ctas_calcorr_statement := ctas_calcorr_statement || 'calcorr(DIFF_' || i || ', DIFF_' || j || ', SDEV_' || i || ', SDEV_' || j || ') CORR_' || i || '_' ||  j;
					FOR j IN 1 .. cols-1
						LOOP
							ctas_calcorr_statement := ctas_calcorr_statement || ', calcorr(DIFF_' || i || ', DIFF_' || j || ', SDEV_' || i || ', SDEV_' || j || ') CORR_' || i || '_' || j;
--                                        		execute immediate ctas_calcorr_statement;
--                                        		RAISE NOTICE 'cal correlation statement is %', ctas_calcorr_statement;
						END LOOP;
					IF i != cols-2 THEN
						ctas_calcorr_statement := ctas_calcorr_statement || ', ';
					END IF;
				END LOOP;
			 ctas_calcorr_statement := ctas_calcorr_statement || ' FROM NZ_DIFF_SDEV_' || rows || '_' || cols || ';';
			execute immediate ctas_calcorr_statement;
                        --RAISE NOTICE 'cal correlation statement is %', ctas_calcorr_statement;
			
			SELECT CURRENT_TIME INTO end_time;
			RAISE NOTICE 'END TIME IS %',end_time;
			
			time_taken := end_time - start_time;
			RAISE NOTICE 'TIME TAKEN IS %',time_taken;
			
			--*******************************************************************************************************************************
			--NETEZZA ANALYTICS FUNCTION: CORR_MATRIX_AGG 
			--*******************************************************************************************************************************
			select current_time INTO start_time;

                        RAISE NOTICE 'START TIME FOR NZA IS %',start_time;

			nza_corr_matrix := 'CREATE TABLE NZA_CORR_' || rows || '_' || cols || ' AS  SELECT nza..CORR_MATRIX_AGG(' || cols || ',' || cols;
			 --j := 0;
                        --nza_corr_matrix := nza_corr_matrix || ', COL_' || j || '.XDiff DIFF_' || j;
                        FOR j IN 0 .. cols-1
                                LOOP
                                        nza_corr_matrix := nza_corr_matrix || ', col' || j;
                                 END LOOP;
			 FOR j IN 0 .. cols-1
                                LOOP
                                        nza_corr_matrix := nza_corr_matrix || ', col' || j;
                                 END LOOP;
			nza_corr_matrix := nza_corr_matrix || ') from NZ_INPUT_TABLE_' || rows || '_' || cols || ';';
			execute immediate nza_corr_matrix;
                        RAISE NOTICE 'nza correlation statement is %', nza_corr_matrix;

			SELECT CURRENT_TIME INTO end_time;
                        RAISE NOTICE 'END TIME FOR NZA IS %',end_time;

			time_taken := end_time - start_time;
                        --SELECT AGE(start_time) INTO time_taken;
                        RAISE NOTICE 'TIME TAKEN FOR NZA IS %',time_taken;

		END;
END_PROC;


