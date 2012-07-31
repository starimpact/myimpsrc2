#
#AR := arm-hisiv100-linux-ar
include Makefile.param


.PHONY : rel clean pea_static

rel:
	@cd AlgoModule/ALGO_PARA/Para; make
	@cd AlgoModule/AVD/; make
	@cd AlgoModule/GA/CommonAlgo/Common; make
	@cd AlgoModule/GA/CommonAlgo/ImageAlgo; make
	@cd AlgoModule/GA/EventManager; make
	@cd AlgoModule/GA/LiscenseManager; make
	@cd AlgoModule/GA/WaterMark; make	
	@cd AlgoModule/PEA/code/BVA; make
	@cd AlgoModule/PEA/code/ObjRecognition/Detector; make
	@cd AlgoModule/PEA/code/ObjRecognition/Tracker; make
	@cd AlgoModule/PEA/code/ObjRecognition; make
	@cd AlgoModule/PEA/code/PEACommon; make
	@cd AlgoModule/PEA/code/System; make

clean:
	@cd AlgoModule/ALGO_PARA/Para; make clean
	@cd AlgoModule/AVD/; make clean
	@cd AlgoModule/GA/CommonAlgo/Common; make clean
	@cd AlgoModule/GA/CommonAlgo/ImageAlgo; make clean
	@cd AlgoModule/GA/EventManager; make clean
	@cd AlgoModule/GA/LiscenseManager; make clean
	@cd AlgoModule/GA/WaterMark; make clean
	@cd AlgoModule/PEA/code/BVA; make clean
	@cd AlgoModule/PEA/code/ObjRecognition/Detector; make clean
	@cd AlgoModule/PEA/code/ObjRecognition/Tracker; make clean
	@cd AlgoModule/PEA/code/ObjRecognition; make clean
	@cd AlgoModule/PEA/code/PEACommon; make clean
	@cd AlgoModule/PEA/code/System; make clean
	@rm -rf *.o
	@rm -rf *.a
	@rm -rf *_EXE
	@rm -rf *.so

PEA_OBJS := AlgoModule/ALGO_PARA/Para/imp_para.o \
			AlgoModule/GA/CommonAlgo/Common/imp_ga_carray.o \
			AlgoModule/GA/CommonAlgo/Common/imp_ga_common.o \
			AlgoModule/GA/CommonAlgo/Common/imp_ga_image_mgr.o \
			AlgoModule/GA/CommonAlgo/Common/imp_ga_math.o \
			AlgoModule/GA/CommonAlgo/Common/imp_ga_memory_mgr.o \
			AlgoModule/GA/CommonAlgo/ImageAlgo/imp_ga_canny_standard.o \
			AlgoModule/GA/CommonAlgo/ImageAlgo/imp_ga_fft.o \
			AlgoModule/GA/CommonAlgo/ImageAlgo/imp_ga_hysteresis.o \
			AlgoModule/GA/CommonAlgo/ImageAlgo/imp_ga_image_algo.o \
			AlgoModule/GA/CommonAlgo/ImageAlgo/imp_ga_integral.o \
			AlgoModule/GA/EventManager/imp_ga_evt_manager.o \
			AlgoModule/GA/LiscenseManager/imp_ga_license_manager.o \
			AlgoModule/GA/WaterMark/imp_pea_watermark.o \
			AlgoModule/PEA/code/BVA/imp_pea_bva.o \
			AlgoModule/PEA/code/BVA/imp_pea_bva_common.o \
			AlgoModule/PEA/code/BVA/imp_pea_bva_log.o \
			AlgoModule/PEA/code/BVA/imp_pea_bva_para.o \
			AlgoModule/PEA/code/BVA/imp_pea_multripwire.o \
			AlgoModule/PEA/code/BVA/imp_pea_perimeter.o \
			AlgoModule/PEA/code/BVA/imp_pea_static.o \
			AlgoModule/PEA/code/BVA/imp_pea_tgt_evt_mgr.o \
			AlgoModule/PEA/code/BVA/imp_pea_tripwire.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_pea_bgm_stabilizer.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_vibe_bgmodel.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_pea_detector.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_pea_detector_para.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_pea_region_extract.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_pea_regionset_common.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_pea_regionset_label.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_pea_regionset_merge.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_pea_regionset_rmabnml.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_pea_regionset_suppress_fg.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_pea_regionset_type.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_pea_shadow_model.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_osc_detect.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_osc_r_detect.o \
			AlgoModule/PEA/code/ObjRecognition/Detector/imp_osc_detect_out.o \
			AlgoModule/PEA/code/ObjRecognition/Tracker/imp_pea_tracker.o \
			AlgoModule/PEA/code/ObjRecognition/Tracker/imp_pea_tracker_common.o \
			AlgoModule/PEA/code/ObjRecognition/Tracker/imp_pea_tracker_cs.o \
			AlgoModule/PEA/code/ObjRecognition/Tracker/imp_pea_tracker_log.o \
			AlgoModule/PEA/code/ObjRecognition/Tracker/imp_pea_tracker_motion.o \
			AlgoModule/PEA/code/ObjRecognition/Tracker/imp_pea_tracker_para.o \
			AlgoModule/PEA/code/ObjRecognition/imp_pea_obj_recognition.o \
			AlgoModule/PEA/code/PEACommon/imp_pea_common.o \
			AlgoModule/PEA/code/System/imp_osc_system.o \
			AlgoModule/PEA/code/System/imp_pea_system.o  


pea_static: $(PEA_OBJS)
	$(AR) rcs libPEA.a $^

pea_arm_static: $(PEA_OBJS)
	$(AR) rcs libArmPEA.a $^

pea_arm_dynamic: $(PEA_OBJS)
	$(CC) -fPIC -o libArmPEA.so $^ -shared
#	gcc -o librh5pea.so $^ -shared
#	arm-hisiv100-linux-gcc -o libA9PEA.so $^ -shared

