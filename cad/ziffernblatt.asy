import geometry;

size(25cm,18cm);

texpreamble("\usepackage[default]{comfortaa}
\usepackage[T1]{fontenc}");

defaultpen(fontsize(25pt));

real ppm2angle(real ppm) {
  real m = (ppm-400.0)/(5000.0-400.0);
  real w = (sqrt(1288*m+81)-9)/28;
  real a = 180-180*w;
  return a;
}


for (int ppm=400; ppm<=5000; ppm+=100) {
  real a = ppm2angle(ppm);
  pen p = currentpen;
  if (ppm%1000==0) {
    p+=linewidth(2mm);
  } else if (ppm%500==0) {
    p+=linewidth(1mm);
  } else {
    p+=linewidth(0.5mm);
  }
  
  
  draw(dir(a)--dir(a)*1.1,p);
  if (ppm%500 == 0) {
    label(format("%d", ppm), dir(a)*0.92, fontsize(20pt));
  }
  label("ppm CO\textsubscript{2}", dir(ppm2angle(1200))*0.55, fontsize(50pt));

  label("Lüften", dir(ppm2angle(1800))*0.8);
  label("So nicht", dir(ppm2angle(2800))*0.72);
  label("Evakuieren", dir(ppm2angle(4000))*0.6);
  label("co2.tech-lab.ch", dir(ppm2angle(400))*0.6);
  label("Ein Projekt des Tech-Lab der Kantonsschule am Burggraben", dir(ppm2angle(300))*0.6);
	
}
