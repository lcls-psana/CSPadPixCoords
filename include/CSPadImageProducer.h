#ifndef CSPADPIXCOORDS_CSPADIMAGEPRODUCER_H
#define CSPADPIXCOORDS_CSPADIMAGEPRODUCER_H

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id$
//
// Description:
//	Class CSPadImageProducer.
//
//------------------------------------------------------------------------

//-----------------
// C/C++ Headers --
//-----------------

//----------------------
// Base Class Headers --
//----------------------
#include "psana/Module.h"
#include "MsgLogger/MsgLogger.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PSCalib/CSPadCalibPars.h"

#include "CSPadPixCoords/QuadParameters.h"
#include "CSPadPixCoords/PixCoords2x1.h"
#include "CSPadPixCoords/PixCoordsQuad.h"
#include "CSPadPixCoords/PixCoordsCSPad.h"

#include "CSPadPixCoords/Image2D.h"
#include "CSPadPixCoords/GlobalMethods.h"

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

#include "PSEvt/Source.h"
//#include "psddl_psana/cspad.ddl.h"

//		---------------------
// 		-- Class Interface --
//		---------------------

using namespace std;

namespace CSPadPixCoords {

/// @addtogroup CSPadPixCoords

/**
 *  @ingroup CSPadPixCoords
 *
 *  @brief CSPadImageProducer produces the CSPad image for each event and add it to the event in psana framework.
 *
 *  CSPadImageProducer works in psana framework. It does a few operation as follows:
 *  \n 1) get the pixel coordinates from PixCoords2x1, PixCoordsQuad, and PixCoordsCSPad classes,
 *  \n 2) get data from the event,
 *  \n 3) produce the Image2D object with CSPad image for each event,
 *  \n 4) add the Image2D object in the event for further modules.
 *
 *  Time consumed to fill the CSPad image array (currently [1750][1750]) 
 *  is measured to be about 40 msec/event on psana0105. 
 *
 *  This class should not be used directly in the code of users modules. 
 *  Instead, it should be added as a module in the psana.cfg file with appropriate parameters.
 *  Then, the produced Image2D object can be extracted from event and used in other modules.
 *
 *  This software was developed for the LCLS project.  If you use all or 
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @see PixCoords2x1, PixCoordsQuad, PixCoordsCSPad, CSPadImageGetTest
 *
 *  @version \$Id$
 *
 *  @author Mikhail S. Dubrovin
 */
    
typedef int16_t pixmap_cspad_t;
typedef int32_t pixnum_cspad_t;


class CSPadImageProducer : public Module {
public:

  enum { NQuadsMax    = Psana::CsPad::MaxQuadsPerSensor  };  // 4
  enum { N2x1         = Psana::CsPad::SectorsPerQuad     };  // 8
  enum { NCols2x1     = Psana::CsPad::ColumnsPerASIC     };  // 185
  enum { NRows2x1     = Psana::CsPad::MaxRowsPerASIC * 2 };  // 388
  enum { SizeOf2x1Arr = NRows2x1 * NCols2x1              };  // 185*388;
  const static uint32_t ARR_SIZE=32*SizeOf2x1Arr;

  /// Default constructor
  CSPadImageProducer (const std::string& name) ;

  /// Destructor
  virtual ~CSPadImageProducer () ;

  /// Method which is called once at the beginning of the job
  virtual void beginJob(Event& evt, Env& env);
  
  /// Method which is called at the beginning of the run
  virtual void beginRun(Event& evt, Env& env);
  
  /// Method which is called at the beginning of the calibration cycle
  virtual void beginCalibCycle(Event& evt, Env& env);
  
  /// Method which is called with event data, this is the only required 
  /// method, all other methods are optional
  virtual void event(Event& evt, Env& env);
  
  /// Method which is called at the end of the calibration cycle
  virtual void endCalibCycle(Event& evt, Env& env);

  /// Method which is called at the end of the run
  virtual void endRun(Event& evt, Env& env);

  /// Method which is called once at the end of the job
  virtual void endJob(Event& evt, Env& env);


protected:

  void printInputParameters();
  void getConfigPars(Env& env);
  bool getGeometryPars(const std::string& calib_dir, const int runnum, const unsigned prbits);
  void getCalibPars(Event& evt, Env& env);
  void procEvent(Event& evt, Env& env);
  void getCSPadConfigFromData(Event& evt);
  void checkTypeImplementation();
  void cspadImgActivePixelMask(Env& env);

  //void cspad_image_init();
  //void cspad_image_fill(const int16_t* data, CSPadPixCoords::QuadParameters* quadpars, PSCalib::CSPadCalibPars *cspad_calibpar);
  //void cspad_image_save_in_file(const std::string &filename = "cspad_image.txt");
  //void cspad_image_add_in_event(Event& evt);

private:

  // Data members, this is for example purposes only

  std::string m_calibDir;       // i.e. ./calib
  std::string m_typeGroupName;  // i.e. CsPad::CalibV1
  std::string m_str_src;        // i.e. CxiDs1.0:Cspad.0
   
  Source      m_source;         // Data source set from config file
  Pds::Src    m_src;
  std::string m_inkey; 
  std::string m_imgkey;         
  std::string m_fname_pixmap;
  std::string m_fname_pixnum;
  std::string m_outtype;
  bool     m_tiltIsApplied;
  unsigned m_print_bits;
  long     m_count;
  long     m_count_cfg;
  long     m_count_msg;
  DATA_TYPE   m_dtype;

  // Parameters form Psana::CsPad::ConfigV# object
  uint32_t m_numQuadsInConfig;
  uint32_t m_roiMask        [4];
  uint32_t m_numAsicsStored [4];

  // Parameters form Psana::CsPad::DataV# and Psana::CsPad::ElementV# object
  uint32_t m_numQuads;
  uint32_t m_quadNumber     [4];
  uint32_t m_num2x1Stored   [4];

  PSCalib::CSPadCalibPars        *m_cspad_calibpar;
  CSPadPixCoords::PixCoords2x1   *m_pix_coords_2x1;
  CSPadPixCoords::PixCoordsQuad  *m_pix_coords_quad;
  CSPadPixCoords::PixCoordsCSPad *m_pix_coords_cspad;

  //uint32_t   m_cspad_ind;
  double    *m_coor_x_pix;
  double    *m_coor_y_pix;
  uint32_t  *m_coor_x_int;
  uint32_t  *m_coor_y_int;

  const static int NX_QUAD=850;
  const static int NY_QUAD=850;

  const static int NX_CSPAD=1750;
  const static int NY_CSPAD=1750;
  const static uint32_t IMG_SIZE=NX_CSPAD*NY_CSPAD;

  //enum{ NX_QUAD=850, 
  //      NY_QUAD=850 };

  //enum{ NX_CSPAD=1750, 
  //      NY_CSPAD=1750,
  //      IMG_SIZE=NX_CSPAD*NY_CSPAD };

  //double m_arr_cspad_image[NX_CSPAD][NY_CSPAD];

//-------------------
  /**
   * @brief Gets m_src, m_numQuadsInConfig, m_roiMask[q] and m_num2x1Stored[q] from the Psana::CsPad::ConfigV# object.
   * 
   */

  template <typename T>
  bool getQuadConfigParsForType(Env& env) {

    //cout  << " Check configuration for source: " << m_source << endl;	     

        boost::shared_ptr<T> config = env.configStore().get(m_source, &m_src);
        if (config.get()) {
            for (uint32_t q = 0; q < NQuadsMax; ++ q) {
              m_roiMask[q]         = config->roiMask(q);
              m_numAsicsStored[q]  = config->numAsicsStored(q);
            }
            m_count_cfg ++;
	    return true;
	}
	return false;
  }

//-------------------
  /**
   * @brief Gets m_numQuads and m_quadNumber[q] from the Psana::CsPad::DataV# and ElementV# objects.
   * 
   */

  template <typename TDATA, typename TELEMENT>
  bool getCSPadConfigFromDataForType(Event& evt) {

    //typedef int16_t data_cspad_t;

    std::string key=""; // FOR RAW CSPAD DATA

    boost::shared_ptr<TDATA> data = evt.get(m_source, key, &m_src);
    if (data.get()) {
      m_numQuads = data->quads_shape()[0];

      for (uint32_t q = 0; q < m_numQuads; ++ q) {
        const TELEMENT& el = data->quads(q);
        m_quadNumber[q]    = el.quad();
        m_num2x1Stored[q]  = el.data().shape()[0];
      }
      return true;
    }
    return false;
  }

//--------------------
  /**
   * @brief Fills a part of the image (img_nda) for one quad per call.
   * 
   * @param[in]  data            pointer to the beginning of the data array for quad.
   * @param[in]  quadpars        pointer to the object with configuration parameters for quad.
   * @param[out] img_nda         reference to the ndarray<T,2> with CSPAD image.
   */
  
  template <typename TIN, typename TOUT>
  void cspadImageFillForType(const TIN* data, CSPadPixCoords::QuadParameters* quadpars, ndarray<TOUT,2>& img_nda) 
  {
        int       quad    = quadpars -> getQuadNumber();
        uint32_t  roiMask = quadpars -> getRoiMask();

        int ind_in_arr = 0;

	for(uint32_t sect=0; sect < N2x1; sect++)
	{
	     bool bitIsOn = roiMask & (1<<sect);
	     if( !bitIsOn ) continue; 

	     int pix_in_cspad = (quad*N2x1 + sect) * SizeOf2x1Arr;
 
             const TIN *data2x1 = &data[ind_in_arr * SizeOf2x1Arr];
             //cout  << "  add section " << sect << endl;	     
 
             for (uint32_t c=0; c<NCols2x1; c++) {
             for (uint32_t r=0; r<NRows2x1; r++) {

               // This access takes 72ms/cspad
               //int ix = (int) m_pix_coords_cspad -> getPixCoor_pix (XCOOR, quad, sect, r, c);
               //int iy = (int) m_pix_coords_cspad -> getPixCoor_pix (YCOOR, quad, sect, r, c);

               // This access takes 40ms/cspad
               int ix = m_coor_x_int [pix_in_cspad];
               int iy = m_coor_y_int [pix_in_cspad];
	       pix_in_cspad++;

	       if(ix <  0)        continue;
	       if(iy <  0)        continue;
	       if(ix >= NX_CSPAD) continue;
	       if(iy >= NY_CSPAD) continue;

	       //if (data2x1[c*NRows2x1+r] != 1 ) cout << " data=" << data2x1[c*NRows2x1+r]  
	       //    << " q:" << quad << " s:" << sect << " r:"  << r << " c:"  <<  c << endl;	    
               //m_arr_cspad_image[ix][iy] += (double)data2x1[c*NRows2x1+r];
               img_nda[ix][iy] += (TOUT)data2x1[c*NRows2x1+r];
             }
             }
             ++ind_in_arr;
 	}
  }

//-------------------
  /**
   * @brief For requested m_source and m_inkey process Psana::CsPad::DataV1, or V2
   * Returns false if data is missing.
   * Output image data type TOUT is equal to input data type TOUT=int16_t.
   */

  template <typename TDATA, typename TELEMENT>
  bool procCSPadDataForType (Event& evt) {

      typedef int16_t data_cspad_t;
      typedef int16_t TOUT; // ok 
      //typedef double  TOUT; // ok
      //typedef float   TOUT; // ok

      boost::shared_ptr<TDATA> data_shp = evt.get(m_source, m_inkey, &m_src); // get m_src here
      
      if (data_shp.get()) {
      
        const unsigned shape[] = {NY_CSPAD,NX_CSPAD};
        ndarray<TOUT,2> img_nda(shape);
        std::fill(img_nda.begin(), img_nda.end(), TOUT(0));    
        //std::fill_n(img_nda.data(), int(IMG_SIZE), TOUT(0));    
        //std::fill_n(&m_arr_cspad_image[0][0], int(IMG_SIZE), double(0));
      
        int nQuads = data_shp->quads_shape()[0];
        for (int q = 0; q < nQuads; ++ q) {
            const TELEMENT& el = data_shp->quads(q);      
            const ndarray<const data_cspad_t,3>& data_nda = el.data();
            uint32_t qNum = el.quad() ;
            CSPadPixCoords::QuadParameters *quadpars = new CSPadPixCoords::QuadParameters(qNum, NX_QUAD, NY_QUAD, m_numAsicsStored[qNum], m_roiMask[qNum]);      

            cspadImageFillForType<data_cspad_t, TOUT>(data_nda.data(), quadpars, img_nda);
        }
      
        //addImageInEventForType<double>(evt, &m_arr_cspad_image[0][0]);
        //addImageInEventForType<TOUT>(evt, img_nda);
        save2DArrayInEvent<TOUT>(evt, m_src, m_imgkey, img_nda);

        return true;
      } // if (data_shp.get())
      return false;
  }

//-------------------
  /**
   * @brief For requested m_source and m_inkey process CSPAD data ndarray<T,3>
   * Returns false if data is missing.
   * Output image data type T is equal to input data type T.
   */

  template <typename T>
    void procCSPadNDArrForTypeAndNDArr (Event& evt, const ndarray<const T,3> inp_ndarr) {
 
          const unsigned shape[] = {NY_CSPAD,NX_CSPAD};
          ndarray<T,2> img_nda(shape);
          //std::fill_n(img_nda.data(), int(IMG_SIZE), T(0));    
          std::fill(img_nda.begin(), img_nda.end(), T(0));    

          //const ndarray<const T,3> inp_ndarr = *shp.get(); //const T* p_data = shp->data();
	  //cout << "inp_ndarr.shape():" << str_shape<const T,3>(inp_ndarr) << '\n';
        
          int ind2x1_in_arr = 0;        

	  if (inp_ndarr.shape()[0] == 32) { // full size array [32, 185, 388]

              for (uint32_t q = 0; q < 4; ++ q) {
	          const T* data_quad = &inp_ndarr[ind2x1_in_arr][0][0]; 
                  CSPadPixCoords::QuadParameters *quadpars = new CSPadPixCoords::QuadParameters(q, NX_QUAD, NY_QUAD, 8, 255);         	      
                  cspadImageFillForType<T,T>(data_quad, quadpars, img_nda);        
                  ind2x1_in_arr += 8;
              }
	  }
	  else { // array shaped as data [N<32, 185, 388]

              for (uint32_t q = 0; q < m_numQuads; ++ q) {
	              const T* data_quad = &inp_ndarr[ind2x1_in_arr][0][0]; 
                  uint32_t qNum = m_quadNumber[q]; 
                  CSPadPixCoords::QuadParameters *quadpars = new CSPadPixCoords::QuadParameters(qNum, NX_QUAD, NY_QUAD, m_numAsicsStored[qNum], m_roiMask[qNum]);         
	      
                  cspadImageFillForType<T,T>(data_quad, quadpars, img_nda);        
                  ind2x1_in_arr += m_num2x1Stored[q];
              }
	  }
        
          //addImageInEventForType<T>(evt, img_nda);
          save2DArrayInEvent<T>(evt, m_src, m_imgkey, img_nda);
  }

//-------------------
  /**
   * @brief For requested m_source and m_inkey process CSPAD data ndarray<T,3>
   * Returns false if data is missing.
   * Output image data type T is equal to input data type T.
   */

  template <typename T>
  bool procCSPadNDArrForType (Event& evt) {
 
        if( m_print_bits & 8 ) MsgLog(name(), info, "Produce image from CSPAD ndarray, source:" << m_source 
    	                                           << " key:" << m_inkey << " data type:" << typeid(T).name() );
        
        boost::shared_ptr< ndarray<const T,3> > shp_const = evt.get(m_source, m_inkey, &m_src); // get m_src here
        if (shp_const.get()) { procCSPadNDArrForTypeAndNDArr<T>(evt, *shp_const.get()); return true; }

        boost::shared_ptr< ndarray<T,3> > shp = evt.get(m_source, m_inkey, &m_src); // get m_src here
        if (shp.get()) { procCSPadNDArrForTypeAndNDArr<T>(evt, *shp.get()); return true; }

        return false;
  }

//-------------------

};

} // namespace CSPadPixCoords

#endif // CSPADPIXCOORDS_CSPADIMAGEPRODUCER_H
