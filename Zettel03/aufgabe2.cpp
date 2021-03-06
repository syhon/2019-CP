#include <iostream>
#include <iomanip>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <math.h>

using namespace std;
using namespace Eigen;

// init hamilton in position representation
MatrixXd hamilton_pos_repr(float delta, float lam, int dim)
{
  int length = 2*dim+1;
  MatrixXd h = MatrixXd::Zero(length, length);
  // non-diagonal
  h.diagonal<1>() = -pow(delta,-2)*ArrayXd::Ones(length-1);
  h.diagonal<-1>() = -pow(delta,-2)*ArrayXd::Ones(length-1);
  // diagonal
  ArrayXd tempvector = ArrayXd::Ones(length);
  for (int i=0; i<length; i++)
  {
    int n = i-dim;
    tempvector(i) = 2/(delta*delta)+delta*delta*n*n+lam*delta*n*delta*n*delta*n*delta*n;
    // tempvector(i) = 2*pow(delta,-2)+delta*delta*n*n+lam*pow(delta,4)*pow(n,4);
  }
  h.diagonal() = tempvector;
  return h;
}

// 10 lowest eigen energys of the anharmonic oszillator
// with the discretisation of space in position representation
ArrayXd ahOszi_pos(int L, float delta, float lam)
{
  // init hamilton
  MatrixXd h = hamilton_pos_repr(delta, lam, L/delta);
  // cout << h.topLeftCorner(15,15) << endl;
  // compute eigenvalues
  VectorXd eivals = h.eigenvalues().real();
  std::sort(eivals.data(), eivals.data()+eivals.size());
  return eivals.head(10);
}

// init hamilton in energy representation
MatrixXd hamilton_en_repr(int N, float lam)
{
  MatrixXd h = MatrixXd::Zero(N+1,N+1);
  // adapt a linear spaced vector to initialise hamilton, dimension N+1
  ArrayXd lins = VectorXd::LinSpaced(N+1,0,N);
  h.diagonal() = (2*lins+1.0)+lam*0.25*(6*lins*(lins+1)+3); // harmonic and anharm part
  h.diagonal<4>() = lam*0.25*sqrt(lins.tail(N-3)*(lins.tail(N-3)-1)*
      (lins.tail(N-3)-2)*(lins.tail(N-3)-3));
  h.diagonal<-4>() = lam*0.25*sqrt((lins.head(N-3)+1)*(lins.head(N-3)+2)*
      (lins.head(N-3)+3)*(lins.head(N-3)+4));
  h.diagonal<2>() = lam*0.25*sqrt(lins.tail(N-1)*(lins.tail(N-1)-1)) *
    (4*lins.tail(N-1)-2);
  h.diagonal<-2>() = lam*0.25*sqrt((lins.head(N-1)+1)*(lins.head(N-1)+2)) *
    (4*lins.head(N-1)+6);
  return h;
}

// 10 lowest eigen energys of the anharmonic oszillator
// with a subspace of the first N states in energy representation
ArrayXd ahOszi_en(int N, float lam)
{
  // init hamilton
  MatrixXd h = hamilton_en_repr(N, lam);
  // cout << h.topLeftCorner(15,15) << endl;
  // compute eigenvalues
  VectorXd eivals = h.eigenvalues().real();
  std::sort(eivals.data(), eivals.data()+eivals.size());
  return eivals.head(10);
}

int main()
{
  cout << "Aufgabe 2" << endl;

  cout << "\tTeil b)" << endl;
  const int L = 10;
  const float delta = 0.1;  // discretization steps
  const float lam = 0.2;  // lambda, distortion
  cout << "\tLength " << L/delta << ", Dimension of Matrix " << 2*L/delta+1 << endl;
  ArrayXd eivals = ahOszi_pos(L, delta, lam);
  cout << "\t10 minor eigenvalues:" << endl << "\t" << eivals.transpose() << endl;

  cout << "\tTeil c)" << endl;
  int N = 50;
  cout << "\tDimension " << N+1 << endl;
  eivals = ahOszi_en(N, lam);
  cout << "\t10 minor eigenvalues:" << endl << "\t" << eivals.transpose() << endl;

  cout << "\tTeil d)" << endl;
  ofstream enfile;
  ofstream posfile;
  string filename_en = "build/aufg2-energy.txt";
  string filename_pos = "build/aufg2-position.txt";
  enfile.open(filename_en, ios::trunc);
  posfile.open(filename_pos, ios::trunc);
  enfile << "n;0;1;2;3;4;5;6;7;8;9" << endl;
  posfile << "n;0;1;2;3;4;5;6;7;8;9" << endl;
  // double n[] = {10, 20, 40, 80, 100, 200, 400};
  // double n[] = {10, 20, 40, 80, 100, 200};
  ArrayXd n = ArrayXd::LinSpaced(20,10,200);
  for (int i=0; i<20; i++)
  {
    enfile << n[i] << ";";
    posfile << n[i] << ";";
    double d = 2*L/n[i];
    VectorXd evEnergy = ahOszi_en(n[i], lam);
    VectorXd evPosition = ahOszi_pos(L, d, lam);
    for (int k=0; k<10; k++){
      enfile << setprecision(40) << evEnergy(k);
      posfile << setprecision(40) << evPosition(k);
      if(k<9){
        enfile << ";";
        posfile << ";";
      }
    }
    enfile << endl;
    posfile << endl;
  }
  enfile.close();
  posfile.close();
  return 0;
}
