////////////////////////////////////////////////////////////////////// 
// pdf/PDFfont.cpp 
// (c) 2000-2007 Goncalo Abecasis
// 
// This file is distributed as part of the MERLIN source code package   
// and may not be redistributed in any form, without prior written    
// permission from the author. Permission is granted for you to       
// modify this file for your own personal use, but modified versions  
// must retain this copyright notice and must not be distributed.     
// 
// Permission is granted for you to use this file to compile MERLIN.    
// 
// All computer programs have bugs. Use this file at your own risk.   
// 
// Tuesday December 18, 2007
// 
 
#include "PDFfont.h"
#include "PDF.h"

PDFFont::PDFFont(PDF & parent) : pdf(parent)
   {
   for (int i = 0; i < 14; i++)
      selectedFonts[i] = 0;

   fontDictionary = -1;
   }

int PDFFont::GetFontID(PDFFonts font, bool bold, bool italic )
   {
   if (font == fSymbol) return 20;
   if (font == fZapfDingbats) return 21;
   return (font * 4 + italic * 2 + bold);
   }

char * PDFFont::GetFontName(PDFFonts font, bool bold, bool italic )
   {
   return fontNames[GetFontID(font, bold, italic)];
   }

char * PDFFont::GetFontName(int fontid)
   {
   return fontNames[fontid];
   }

int PDFFont::TextWidth(int fontid, const char * text)
   {
   int width = 0;

   while (*text)
      width += metrics[fontid][int(*text++)];

   return width;
   }

void PDFFont::WriteDictionary()
   {
   if (fontDictionary == -1)
      return;

   IntArray objects(14);
   objects.Zero();

   for (int i = 0; i < 14; i++)
      if (selectedFonts[i])
         {
         objects[i] = pdf.GetObject();

         pdf.OpenObject(objects[i]);
         pdf.OpenDictionary();
         pdf.WriteName("Type", "Font");
         pdf.WriteName("Subtype", "Type1");
         pdf.WriteName("BaseFont", GetFontName(i));
         pdf.CloseDictionary();
         pdf.CloseObject();
         }

   pdf.OpenObject(fontDictionary);
   pdf.OpenDictionary();
   for (int i = 0; i < 14; i++)
      if (selectedFonts[i])
         {
         char name[4] = {'F', (char) ('0' + i / 10), (char) ('0' + i % 10), 0};
         pdf.WriteReference(name, objects[i]);
         }
   pdf.CloseDictionary();
   pdf.CloseObject();
   }

void PDFFont::SelectFont(int fontid, double size)
   {
   if (fontDictionary == -1)
      fontDictionary = pdf.GetObject();

   pdf.AppendToStream("/F%02d %.1f Tf\n", fontid, size);

   selectedFonts[fontid] = true;
   }

void PDFFont::WriteResources()
   {
   if (fontDictionary != -1)
      pdf.WriteReference("Font", fontDictionary);
   }

void PDFFont::MarkFont(int font)
   {
   if (fontDictionary == -1)
      fontDictionary = pdf.GetObject();

   selectedFonts[font] = true;
   }

char * PDFFont::fontNames[14] = { "Times-Roman",
                                  "Times-Bold",
                                  "Times-Italic",
                                  "Times-BoldItalic",
                                  "Helvetica",
                                  "Helvetica-Bold",
                                  "Helvetica-Oblique",
                                  "Helvetica-BoldOblique",
                                  "Courier",
                                  "Courier-Bold",
                                  "Courier-Oblique",
                                  "Courier-BoldOblique",
                                  "Symbol",
                                  "ZapfDingbats" };

int PDFFont::metrics[14][256] = {
  /* Times-Roman -- Widths for 149 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 250, 333, 408, 500, 500, 833, 778, 333, 333, 333,
   500, 564, 250, 333, 250, 278, 500, 500, 500, 500, 500, 500, 500, 500,
   500, 500, 278, 278, 564, 564, 564, 444, 921, 722, 667, 667, 722, 611,
   556, 722, 722, 333, 389, 722, 611, 889, 722, 722, 556, 722, 667, 556,
   611, 722, 722, 944, 722, 722, 611, 333, 278, 333, 469, 500, 333, 444,
   500, 444, 500, 444, 333, 500, 500, 278, 278, 500, 278, 778, 500, 500,
   500, 500, 333, 389, 278, 500, 500, 722, 500, 500, 444, 480, 200, 480,
   541,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 333, 500, 500, 167, 500, 500, 500,
   500, 180, 444, 500, 333, 333, 556, 556,  -1, 500, 500, 500, 250,  -1,
   453, 350, 333, 444, 444, 500, 1000, 1000,  -1, 444,  -1, 333, 333, 333,
   333, 333, 333, 333, 333,  -1, 333, 333,  -1, 333, 333, 333, 1000,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1, 889,  -1, 276,  -1,  -1,  -1,  -1, 611, 722, 889, 310,  -1,  -1,
    -1,  -1,  -1, 667,  -1,  -1,  -1, 278,  -1,  -1, 278, 500, 722, 500,
    -1,  -1,  -1,  -1 },
  /* Times-Bold -- Widths for 149 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 250, 333, 555, 500, 500, 1000, 833, 333, 333, 333,
   500, 570, 250, 333, 250, 278, 500, 500, 500, 500, 500, 500, 500, 500,
   500, 500, 333, 333, 570, 570, 570, 500, 930, 722, 667, 722, 722, 667,
   611, 778, 778, 389, 500, 778, 667, 944, 722, 778, 611, 778, 722, 556,
   667, 722, 722, 1000, 722, 722, 667, 333, 278, 333, 581, 500, 333, 500,
   556, 444, 556, 444, 333, 500, 556, 278, 333, 556, 278, 833, 556, 500,
   556, 556, 444, 389, 333, 556, 500, 722, 500, 500, 444, 394, 220, 394,
   520,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 333, 500, 500, 167, 500, 500, 500,
   500, 278, 500, 500, 333, 333, 556, 556,  -1, 500, 500, 500, 250,  -1,
   540, 350, 333, 500, 500, 500, 1000, 1000,  -1, 500,  -1, 333, 333, 333,
   333, 333, 333, 333, 333,  -1, 333, 333,  -1, 333, 333, 333, 1000,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1, 1000,  -1, 300,  -1,  -1,  -1,  -1, 667, 778, 1000, 330,  -1,  -1,
    -1,  -1,  -1, 722,  -1,  -1,  -1, 278,  -1,  -1, 278, 500, 722, 556,
    -1,  -1,  -1,  -1 },
  /* Times-Italic -- Widths for 149 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 250, 333, 420, 500, 500, 833, 778, 333, 333, 333,
   500, 675, 250, 333, 250, 278, 500, 500, 500, 500, 500, 500, 500, 500,
   500, 500, 333, 333, 675, 675, 675, 500, 920, 611, 611, 667, 722, 611,
   611, 722, 722, 333, 444, 667, 556, 833, 667, 722, 611, 722, 611, 500,
   556, 722, 611, 833, 611, 556, 556, 389, 278, 389, 422, 500, 333, 500,
   500, 444, 500, 444, 278, 500, 500, 278, 278, 444, 278, 722, 500, 500,
   500, 500, 389, 389, 278, 500, 444, 667, 444, 444, 389, 400, 275, 400,
   541,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 389, 500, 500, 167, 500, 500, 500,
   500, 214, 556, 500, 333, 333, 500, 500,  -1, 500, 500, 500, 250,  -1,
   523, 350, 333, 556, 556, 500, 889, 1000,  -1, 500,  -1, 333, 333, 333,
   333, 333, 333, 333, 333,  -1, 333, 333,  -1, 333, 333, 333, 889,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1, 889,  -1, 276,  -1,  -1,  -1,  -1, 556, 722, 944, 310,  -1,  -1,
    -1,  -1,  -1, 667,  -1,  -1,  -1, 278,  -1,  -1, 278, 500, 667, 500,
    -1,  -1,  -1,  -1 },
  /* Times-BoldItalic -- Widths for 149 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 250, 389, 555, 500, 500, 833, 778, 333, 333, 333,
   500, 570, 250, 333, 250, 278, 500, 500, 500, 500, 500, 500, 500, 500,
   500, 500, 333, 333, 570, 570, 570, 500, 832, 667, 667, 667, 722, 667,
   667, 722, 778, 389, 500, 667, 611, 889, 722, 722, 611, 722, 667, 556,
   611, 722, 667, 889, 667, 611, 611, 333, 278, 333, 570, 500, 333, 500,
   500, 444, 500, 444, 333, 500, 556, 278, 278, 500, 278, 778, 556, 500,
   500, 500, 389, 389, 278, 556, 444, 667, 500, 444, 389, 348, 220, 348,
   570,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 389, 500, 500, 167, 500, 500, 500,
   500, 278, 500, 500, 333, 333, 556, 556,  -1, 500, 500, 500, 250,  -1,
   500, 350, 333, 500, 500, 500, 1000, 1000,  -1, 500,  -1, 333, 333, 333,
   333, 333, 333, 333, 333,  -1, 333, 333,  -1, 333, 333, 333, 1000,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1, 944,  -1, 266,  -1,  -1,  -1,  -1, 611, 722, 944, 300,  -1,  -1,
    -1,  -1,  -1, 722,  -1,  -1,  -1, 278,  -1,  -1, 278, 500, 722, 500,
    -1,  -1,  -1,  -1 },
  /* Helvetica -- Widths for 149 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 278, 278, 355, 556, 556, 889, 667, 222, 333, 333,
   389, 584, 278, 333, 278, 278, 556, 556, 556, 556, 556, 556, 556, 556,
   556, 556, 278, 278, 584, 584, 584, 556, 1015, 667, 667, 722, 722, 667,
   611, 778, 722, 278, 500, 667, 556, 833, 722, 778, 667, 778, 722, 667,
   611, 722, 667, 944, 667, 667, 611, 278, 278, 278, 469, 556, 222, 556,
   556, 500, 556, 556, 278, 556, 556, 222, 222, 500, 222, 833, 556, 556,
   556, 556, 333, 500, 278, 556, 500, 722, 500, 500, 500, 334, 260, 334,
   584,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 333, 556, 556, 167, 556, 556, 556,
   556, 191, 333, 556, 333, 333, 500, 500,  -1, 556, 556, 556, 278,  -1,
   537, 350, 222, 333, 333, 556, 1000, 1000,  -1, 611,  -1, 333, 333, 333,
   333, 333, 333, 333, 333,  -1, 333, 333,  -1, 333, 333, 333, 1000,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1, 1000,  -1, 370,  -1,  -1,  -1,  -1, 556, 778, 1000, 365,  -1,  -1,
    -1,  -1,  -1, 889,  -1,  -1,  -1, 278,  -1,  -1, 222, 611, 944, 611,
    -1,  -1,  -1,  -1 },
  /* Helvetica-Bold -- Widths for 149 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 278, 333, 474, 556, 556, 889, 722, 278, 333, 333,
   389, 584, 278, 333, 278, 278, 556, 556, 556, 556, 556, 556, 556, 556,
   556, 556, 333, 333, 584, 584, 584, 611, 975, 722, 722, 722, 722, 667,
   611, 778, 722, 278, 556, 722, 611, 833, 722, 778, 667, 778, 722, 667,
   611, 722, 667, 944, 667, 667, 611, 333, 278, 333, 584, 556, 278, 556,
   611, 556, 611, 556, 333, 611, 611, 278, 278, 556, 278, 889, 611, 611,
   611, 611, 389, 556, 333, 611, 556, 778, 556, 556, 500, 389, 280, 389,
   584,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 333, 556, 556, 167, 556, 556, 556,
   556, 238, 500, 556, 333, 333, 611, 611,  -1, 556, 556, 556, 278,  -1,
   556, 350, 278, 500, 500, 556, 1000, 1000,  -1, 611,  -1, 333, 333, 333,
   333, 333, 333, 333, 333,  -1, 333, 333,  -1, 333, 333, 333, 1000,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1, 1000,  -1, 370,  -1,  -1,  -1,  -1, 611, 778, 1000, 365,  -1,  -1,
    -1,  -1,  -1, 889,  -1,  -1,  -1, 278,  -1,  -1, 278, 611, 944, 611,
    -1,  -1,  -1,  -1 },
  /* Helvetica-Oblique -- Widths for 149 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 278, 278, 355, 556, 556, 889, 667, 222, 333, 333,
   389, 584, 278, 333, 278, 278, 556, 556, 556, 556, 556, 556, 556, 556,
   556, 556, 278, 278, 584, 584, 584, 556, 1015, 667, 667, 722, 722, 667,
   611, 778, 722, 278, 500, 667, 556, 833, 722, 778, 667, 778, 722, 667,
   611, 722, 667, 944, 667, 667, 611, 278, 278, 278, 469, 556, 222, 556,
   556, 500, 556, 556, 278, 556, 556, 222, 222, 500, 222, 833, 556, 556,
   556, 556, 333, 500, 278, 556, 500, 722, 500, 500, 500, 334, 260, 334,
   584,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 333, 556, 556, 167, 556, 556, 556,
   556, 191, 333, 556, 333, 333, 500, 500,  -1, 556, 556, 556, 278,  -1,
   537, 350, 222, 333, 333, 556, 1000, 1000,  -1, 611,  -1, 333, 333, 333,
   333, 333, 333, 333, 333,  -1, 333, 333,  -1, 333, 333, 333, 1000,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1, 1000,  -1, 370,  -1,  -1,  -1,  -1, 556, 778, 1000, 365,  -1,  -1,
    -1,  -1,  -1, 889,  -1,  -1,  -1, 278,  -1,  -1, 222, 611, 944, 611,
    -1,  -1,  -1,  -1 },
  /* Helvetica-BoldOblique -- Widths for 149 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 278, 333, 474, 556, 556, 889, 722, 278, 333, 333,
   389, 584, 278, 333, 278, 278, 556, 556, 556, 556, 556, 556, 556, 556,
   556, 556, 333, 333, 584, 584, 584, 611, 975, 722, 722, 722, 722, 667,
   611, 778, 722, 278, 556, 722, 611, 833, 722, 778, 667, 778, 722, 667,
   611, 722, 667, 944, 667, 667, 611, 333, 278, 333, 584, 556, 278, 556,
   611, 556, 611, 556, 333, 611, 611, 278, 278, 556, 278, 889, 611, 611,
   611, 611, 389, 556, 333, 611, 556, 778, 556, 556, 500, 389, 280, 389,
   584,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 333, 556, 556, 167, 556, 556, 556,
   556, 238, 500, 556, 333, 333, 611, 611,  -1, 556, 556, 556, 278,  -1,
   556, 350, 278, 500, 500, 556, 1000, 1000,  -1, 611,  -1, 333, 333, 333,
   333, 333, 333, 333, 333,  -1, 333, 333,  -1, 333, 333, 333, 1000,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1, 1000,  -1, 370,  -1,  -1,  -1,  -1, 611, 778, 1000, 365,  -1,  -1,
    -1,  -1,  -1, 889,  -1,  -1,  -1, 278,  -1,  -1, 278, 611, 944, 611,
    -1,  -1,  -1,  -1 },
  /* Courier -- Widths for 149 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600,  -1, 600, 600, 600, 600,  -1,
   600, 600, 600, 600, 600, 600, 600, 600,  -1, 600,  -1, 600, 600, 600,
   600, 600, 600, 600, 600,  -1, 600, 600,  -1, 600, 600, 600, 600,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1, 600,  -1, 600,  -1,  -1,  -1,  -1, 600, 600, 600, 600,  -1,  -1,
    -1,  -1,  -1, 600,  -1,  -1,  -1, 600,  -1,  -1, 600, 600, 600, 600,
    -1,  -1,  -1,  -1 },
  /* Courier-Bold -- Widths for 149 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600,  -1, 600, 600, 600, 600,  -1,
   600, 600, 600, 600, 600, 600, 600, 600,  -1, 600,  -1, 600, 600, 600,
   600, 600, 600, 600, 600,  -1, 600, 600,  -1, 600, 600, 600, 600,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1, 600,  -1, 600,  -1,  -1,  -1,  -1, 600, 600, 600, 600,  -1,  -1,
    -1,  -1,  -1, 600,  -1,  -1,  -1, 600,  -1,  -1, 600, 600, 600, 600,
    -1,  -1,  -1,  -1 },
  /* Courier-Oblique -- Widths for 149 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600,  -1, 600, 600, 600, 600,  -1,
   600, 600, 600, 600, 600, 600, 600, 600,  -1, 600,  -1, 600, 600, 600,
   600, 600, 600, 600, 600,  -1, 600, 600,  -1, 600, 600, 600, 600,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1, 600,  -1, 600,  -1,  -1,  -1,  -1, 600, 600, 600, 600,  -1,  -1,
    -1,  -1,  -1, 600,  -1,  -1,  -1, 600,  -1,  -1, 600, 600, 600, 600,
    -1,  -1,  -1,  -1 },
  /* Courier-BoldOblique -- Widths for 149 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600,
   600,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 600, 600, 600, 600, 600, 600, 600,
   600, 600, 600, 600, 600, 600, 600, 600,  -1, 600, 600, 600, 600,  -1,
   600, 600, 600, 600, 600, 600, 600, 600,  -1, 600,  -1, 600, 600, 600,
   600, 600, 600, 600, 600,  -1, 600, 600,  -1, 600, 600, 600, 600,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1, 600,  -1, 600,  -1,  -1,  -1,  -1, 600, 600, 600, 600,  -1,  -1,
    -1,  -1,  -1, 600,  -1,  -1,  -1, 600,  -1,  -1, 600, 600, 600, 600,
    -1,  -1,  -1,  -1 },
  /* Symbol -- Widths for 189 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 250, 333, 713, 500, 549, 833, 778, 439, 333, 333,
   500, 549, 250, 549, 250, 278, 500, 500, 500, 500, 500, 500, 500, 500,
   500, 500, 278, 278, 549, 549, 549, 444, 549, 722, 667, 722, 612, 611,
   763, 603, 722, 333, 631, 722, 686, 889, 722, 722, 768, 741, 556, 592,
   611, 690, 439, 768, 645, 795, 611, 333, 863, 333, 658, 500, 500, 631,
   549, 549, 494, 439, 521, 411, 603, 329, 603, 549, 549, 576, 521, 549,
   549, 521, 549, 603, 439, 576, 713, 686, 493, 686, 494, 480, 200, 480,
   549,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1, 750, 620, 247, 549, 167, 713, 500, 753,
   753, 753, 753, 1042, 987, 603, 987, 603, 400, 549, 411, 549, 549, 713,
   494, 460, 549, 549, 549, 549, 1000, 603, 1000, 658, 823, 686, 795, 987,
   768, 768, 823, 768, 768, 713, 713, 713, 713, 713, 713, 713, 768, 713,
   790, 790, 890, 823, 549, 250, 713, 603, 603, 1042, 987, 603, 987, 603,
   494, 329, 790, 790, 786, 713, 384, 384, 384, 384, 384, 384, 494, 494,
   494, 494,  -1, 329, 274, 686, 686, 686, 384, 384, 384, 384, 384, 384,
   494, 494, 494,  -1 },
  /* ZapfDingbats -- Widths for 202 characters */
  { -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1, 278, 974, 961, 974, 980, 719, 789, 790, 791, 690,
   960, 939, 549, 855, 911, 933, 911, 945, 974, 755, 846, 762, 761, 571,
   677, 763, 760, 759, 754, 494, 552, 537, 577, 692, 786, 788, 788, 790,
   793, 794, 816, 823, 789, 841, 823, 833, 816, 831, 923, 744, 723, 749,
   790, 792, 695, 776, 768, 792, 759, 707, 708, 682, 701, 826, 815, 789,
   789, 707, 687, 696, 689, 786, 787, 713, 791, 785, 791, 873, 761, 762,
   762, 759, 759, 892, 892, 788, 784, 438, 138, 277, 415, 392, 392, 668,
   668,  -1, 390, 390, 317, 317, 276, 276, 509, 509, 410, 410, 234, 234,
   334, 334,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
    -1,  -1,  -1,  -1,  -1,  -1,  -1, 732, 544, 544, 910, 667, 760, 760,
   776, 595, 694, 626, 788, 788, 788, 788, 788, 788, 788, 788, 788, 788,
   788, 788, 788, 788, 788, 788, 788, 788, 788, 788, 788, 788, 788, 788,
   788, 788, 788, 788, 788, 788, 788, 788, 788, 788, 788, 788, 788, 788,
   788, 788, 894, 838, 1016, 458, 748, 924, 748, 918, 927, 928, 928, 834,
   873, 828, 924, 924, 917, 930, 931, 463, 883, 836, 836, 867, 867, 696,
   696, 874,  -1, 874, 760, 946, 771, 865, 771, 888, 967, 888, 831, 873,
   927, 970, 918,  -1 }};

      
 
 
 
