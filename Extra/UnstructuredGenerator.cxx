#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkUnsignedShortArray.h>
#include <vtkUnstructuredGrid.h>

#define PI 3.14159265358979323846

//-----------------------------------------------------------------------------
void PillarGridGenerator(int rank, int wholeExtent[6], int extent[6], 
                         vtkUnstructuredGrid* grid)
{
  double nx = wholeExtent[1] - wholeExtent[0] + 1;
  double ny = wholeExtent[3] - wholeExtent[2] + 1;
  double nz = wholeExtent[5] - wholeExtent[4] + 1;

  int lnx = extent[1] - extent[0] + 1;
  int lny = extent[3] - extent[2] + 1;
  int lnz = extent[5] - extent[4] + 1;

  vtkNew<vtkPoints> points;
  points->Allocate(lnx * lny * lnz * 8);

  vtkNew<vtkCellArray> hexs;
  hexs->Allocate(lnx * lny * lnz * 9);

  vtkNew<vtkFloatArray> dist_array;
  dist_array->SetName("Distance2Origin");
  dist_array->Allocate(lnx * lny * lnz);

  vtkNew<vtkUnsignedShortArray> rank_array;
  rank_array->SetName("Rank");
  rank_array->Allocate(lnx * lny * lnz);

  vtkIdType p_index_p4 = 0;
  vtkIdType p_index_p5 = 0;
  vtkIdType p_index_p6 = 0;
  vtkIdType p_index_p7 = 0;

  double coef_noise = 0.2;

#define GetZShift(_k, _shift) (shift_faille + _shift + (_k) + coef_noise * rand() / RAND_MAX)
#define GetZShift1(_k) GetZShift(_k, shift_z1)
#define GetZShift2(_k) GetZShift(_k, shift_z2)

  for (int i = extent[0]; i <= extent[1]; i++)
    {
    double shift_z1 = nz * 0.5 * sin((static_cast<double>(i) + 1.) * PI / nx);
    double shift_z2 = nz * 0.5 * sin((static_cast<double>(i) + 2.) * PI / nx);
    double shift_faille = (i > nx/2) ? 0. : nz * 0.3333;

    for (int j = extent[2]; j <= extent[3]; j++)
      {
      for (int k = extent[4]; k <= extent[5]; k++)
        {        
        vtkIdType indice[8];
        // P0
        if (k > extent[4])
          {
          indice[0] = p_index_p4;
          indice[1] = p_index_p5;
          indice[2] = p_index_p6;
          indice[3] = p_index_p7;
          }
        else
          {
          indice[0] = points->InsertNextPoint(i - 0.5, j - 0.5, GetZShift1(k - 0.5));
          indice[1] = points->InsertNextPoint(i + 0.5, j - 0.5, GetZShift2(k - 0.5));
          indice[2] = points->InsertNextPoint(i + 0.5, j + 0.5, GetZShift2(k - 0.5));
          indice[3] = points->InsertNextPoint(i - 0.5, j + 0.5, GetZShift1(k - 0.5));
          }

        indice[4] = points->InsertNextPoint(i - 0.5, j - 0.5, GetZShift1(k + 0.5));
        indice[5] = points->InsertNextPoint(i + 0.5, j - 0.5, GetZShift2(k + 0.5));
        indice[6] = points->InsertNextPoint(i + 0.5, j + 0.5, GetZShift2(k + 0.5));
        indice[7] = points->InsertNextPoint(i - 0.5, j + 0.5, GetZShift1(k + 0.5));
        hexs->InsertNextCell(8, indice);

        p_index_p4 = indice[4];
        p_index_p5 = indice[5];
        p_index_p6 = indice[6];
        p_index_p7 = indice[7];

        double dist = sqrt(static_cast<double>(i*i + j*j + k*k));
        dist_array->InsertNextTuple1(dist);
        rank_array->InsertNextTuple1(rank);
        }
      }
    }

  grid->SetPoints(points.GetPointer());
  grid->SetCells(VTK_HEXAHEDRON, hexs.GetPointer());
  grid->GetCellData()->AddArray(dist_array.GetPointer());
  grid->GetCellData()->AddArray(rank_array.GetPointer());
}

//-----------------------------------------------------------------------------
void DiscontinuousGridGenerator(int rank, int wholeExtent[6], int extent[6],
                                vtkUnstructuredGrid* grid)
{
  srand(1);

  double nx = wholeExtent[1] - wholeExtent[0] + 1;
  double ny = wholeExtent[3] - wholeExtent[2] + 1;
  double nz = wholeExtent[5] - wholeExtent[4] + 1;

  int lnx = extent[1] - extent[0] + 1;
  int lny = extent[3] - extent[2] + 1;
  int lnz = extent[5] - extent[4] + 1;

  vtkNew<vtkPoints> points;
  points->Allocate(lnx * lny * lnz * 8);

  vtkNew<vtkCellArray> hexs;
  hexs->Allocate(lnx * lny * lnz * 9);

  vtkNew<vtkFloatArray> dist_array;
  dist_array->SetName("Distance2Origin");
  dist_array->Allocate(lnx * lny * lnz);

  vtkNew<vtkUnsignedShortArray> rank_array;
  rank_array->SetName("Rank");
  rank_array->Allocate(lnx * lny * lnz);

  double z = 0;
  int p_index = 0;
  double coef_noise = 0.2;

#define GetZShift(_k, _shift) (shift_faille + _shift + (_k) + coef_noise * rand() / RAND_MAX)
#define GetZShift1(_k) GetZShift(_k, shift_z1)
#define GetZShift2(_k) GetZShift(_k, shift_z2)

  for (int i = extent[0]; i <= extent[1]; i++)
    {
    double shift_z1 = nz * 0.5 * sin(((double)i + 1.) * PI / nx);
    double shift_z2 = nz * 0.5 * sin(((double)i + 2.) * PI / nx);
    double shift_faille = (i > nx / 2) ? 0. : nz * 0.3333;

    for (int j = extent[2]; j <= extent[3]; j++)
      {
      for (int k = extent[4]; k <= extent[5]; k++)
        {
        vtkIdType indice[8];
        indice[0] = points->InsertNextPoint(i - 0.5, j - 0.5, GetZShift1(k - 0.5));
        indice[1] = points->InsertNextPoint(i + 0.5, j - 0.5, GetZShift2(k - 0.5));
        indice[2] = points->InsertNextPoint(i + 0.5, j + 0.5, GetZShift2(k - 0.5));
        indice[3] = points->InsertNextPoint(i  -0.5, j + 0.5, GetZShift1(k - 0.5));
        indice[4] = points->InsertNextPoint(i - 0.5, j - 0.5, GetZShift1(k + 0.5));
        indice[5] = points->InsertNextPoint(i + 0.5, j - 0.5, GetZShift2(k + 0.5));
        indice[6] = points->InsertNextPoint(i + 0.5 ,j + 0.5, GetZShift2(k + 0.5));
        indice[7] = points->InsertNextPoint(i - 0.5, j + 0.5, GetZShift1(k + 0.5));
        hexs->InsertNextCell(8, indice);

        double dist = sqrt(static_cast<double>(i*i + j*j + k*k));
        dist_array->InsertNextTuple1(dist);
        rank_array->InsertNextTuple1(rank);
        }
      }
    }

  grid->SetPoints(points.GetPointer());
  grid->SetCells(VTK_HEXAHEDRON, hexs.GetPointer());
  grid->GetCellData()->AddArray(dist_array.GetPointer());
  grid->GetCellData()->AddArray(rank_array.GetPointer());
}

//-----------------------------------------------------------------------------
void ContinuousGridGenerator(int rank, int wholeExtent[6], int extent[6],
                             vtkUnstructuredGrid* grid)
{
  double nx = wholeExtent[1] - wholeExtent[0] + 1;
  double ny = wholeExtent[3] - wholeExtent[2] + 1;
  double nz = wholeExtent[5] - wholeExtent[4] + 1;

  int lnx = extent[1] - extent[0] + 1;
  int lny = extent[3] - extent[2] + 1;
  int lnz = extent[5] - extent[4] + 1;

  int npx = lnx + 1;
  int npy = lny + 1;
  int npz = lnz + 1;

  vtkNew<vtkPoints> points;
  points->Allocate(npx * npy * npz);

  vtkNew<vtkCellArray> hexs;
  hexs->Allocate(lnx * lny * lnz * 9);

  vtkNew<vtkFloatArray> dist_array;
  dist_array->SetName("Distance2Origin");
  dist_array->Allocate(lnx * lny * lnz);

  vtkNew<vtkUnsignedShortArray> rank_array;
  rank_array->SetName("Rank");
  rank_array->Allocate(lnx * lny * lnz);

  for (int i = extent[0]; i <= extent[1] + 1; i++)
    {
    double shift_z = nz * 0.5 * sin(((double)i + 1.) * PI / nx);

    for (int j = extent[2]; j <= extent[3] + 1; j++)
      {
      for (int k = extent[4]; k <= extent[5] + 1; k++)
        {
        double z = k + shift_z;
        points->InsertNextPoint(i, j, z);
        }
      }
    }

  for (int i = extent[0], ii = 0; i <= extent[1]; i++, ii++)
    {
    for (int j = extent[2], jj = 0; j <= extent[3]; j++, jj++)
      {
      for (int k = extent[4], kk = 0; k <= extent[5]; k++, kk++)
        {        
        vtkIdType indice[8];
        indice[0] = (kk  ) + (jj  ) * (lnz + 1) + (ii  ) * (lnz + 1) * (lny + 1);
        indice[1] = (kk+1) + (jj  ) * (lnz + 1) + (ii  ) * (lnz + 1) * (lny + 1);
        indice[2] = (kk+1) + (jj+1) * (lnz + 1) + (ii  ) * (lnz + 1) * (lny + 1);
        indice[3] = (kk  ) + (jj+1) * (lnz + 1) + (ii  ) * (lnz + 1) * (lny + 1);
        indice[4] = (kk  ) + (jj  ) * (lnz + 1) + (ii+1) * (lnz + 1) * (lny + 1);
        indice[5] = (kk+1) + (jj  ) * (lnz + 1) + (ii+1) * (lnz + 1) * (lny + 1);
        indice[6] = (kk+1) + (jj+1) * (lnz + 1) + (ii+1) * (lnz + 1) * (lny + 1);
        indice[7] = (kk  ) + (jj+1) * (lnz + 1) + (ii+1) * (lnz + 1) * (lny + 1);
        hexs->InsertNextCell(8, indice);

        double dist = sqrt(static_cast<double>(i*i + j*j + k*k));
        dist_array->InsertNextTuple1(dist);
        rank_array->InsertNextTuple1(rank);
        }
      }
    }

  grid->SetPoints(points.GetPointer());
  grid->SetCells(VTK_HEXAHEDRON, hexs.GetPointer());
  grid->GetCellData()->AddArray(dist_array.GetPointer());
  grid->GetCellData()->AddArray(rank_array.GetPointer());
}

//-----------------------------------------------------------------------------
void StepsGridGenerator(int rank, int wholeExtent[6], int extent[6],
                        vtkUnstructuredGrid* grid, int nsteps)
{
  srand(1);

  double nx = wholeExtent[1] - wholeExtent[0] + 1;
  double ny = wholeExtent[3] - wholeExtent[2] + 1;
  double nz = wholeExtent[5] - wholeExtent[4] + 1;

  int lnx = extent[1] - extent[0] + 1;
  int lny = extent[3] - extent[2] + 1;
  int lnz = extent[5] - extent[4] + 1;

  int lstep = (nx / nsteps);

  vtkNew<vtkPoints> points;

  vtkNew<vtkCellArray> hexs;
  hexs->Allocate(lnx * lny * lnz * 9);

  vtkNew<vtkFloatArray> dist_array;
  dist_array->SetName("Distance2Origin");
  dist_array->Allocate(lnx * lny * lnz);

  vtkNew<vtkUnsignedShortArray> rank_array;
  rank_array->SetName("Rank");
  rank_array->Allocate(lnx * lny * lnz);

  // Index cells
  int icmin = 0;
  int icmax = 0;
  // Index (1D cooordinate) point
  int ipmin = -1;
  int ipmax = -1;

  double coef_noise = 0.1111;
  double coef_fault = 0.3333;

  int firstipmin = -1;

  for (int istep = 0; istep < nsteps; istep++)
    {
    double shift_step = nz * coef_noise * rand() / RAND_MAX;
    double shift_fault = 0.;

    if (istep > nsteps / 2)
      {
      shift_fault = nz * coef_fault;
      }

    if (istep == nsteps - 1)
      {
      // last steps
      icmax = nx - 1;
      }
    else
      {
      icmax = icmin + lstep - 1;
      }

    ipmin = ipmax + 1;
    ipmax = ipmin + (icmax - icmin)  + 1;

    int licmin = std::max(icmin, extent[0]);
    int licmax = std::min(icmax, extent[1]);
    if (licmin <= licmax)
      {
      int lipmin = ipmin + (licmin - icmin);
      int lipmax = ipmax - (licmax - icmax);
      if (firstipmin == -1)
        {
        firstipmin = lipmin;
        }

      for (int i = lipmin; i <= lipmax; i++)
        {
        for (int j = extent[2]; j <= extent[3] + 1; j++)
          {
          for (int k = extent[4]; k <= extent[5] + 1; k++)
            {
            double x = i - istep;
            double y = j;
            double shift_bf = nz * 0.5 * sin((x+1.) * PI / nx);
            double z = k + shift_bf + shift_step + shift_fault;
            shift_bf = 0.;
            points->InsertNextPoint(x, y, z);
            }
          }
        }

      for (int i = licmin; i <= licmax; i++)
        {
        for (int j = extent[2], jj = 0; j <= extent[3]; j++, jj++)
          {
          for (int k = extent[4], kk = 0; k <= extent[5]; k++, kk++)
            {
            int ii = (lipmin - firstipmin) + i - licmin;

            vtkIdType indice[8];
            indice[0] = (kk  ) + (jj  ) * (lnz + 1) + (ii  ) * (lnz + 1) * (lny + 1);
            indice[1] = (kk+1) + (jj  ) * (lnz + 1) + (ii  ) * (lnz + 1) * (lny + 1);
            indice[2] = (kk+1) + (jj+1) * (lnz + 1) + (ii  ) * (lnz + 1) * (lny + 1);
            indice[3] = (kk  ) + (jj+1) * (lnz + 1) + (ii  ) * (lnz + 1) * (lny + 1);
            indice[4] = (kk  ) + (jj  ) * (lnz + 1) + (ii+1) * (lnz + 1) * (lny + 1);
            indice[5] = (kk+1) + (jj  ) * (lnz + 1) + (ii+1) * (lnz + 1) * (lny + 1);
            indice[6] = (kk+1) + (jj+1) * (lnz + 1) + (ii+1) * (lnz + 1) * (lny + 1);
            indice[7] = (kk  ) + (jj+1) * (lnz + 1) + (ii+1) * (lnz + 1) * (lny + 1);
            hexs->InsertNextCell(8, indice);

            double dist = sqrt(static_cast<double>(i*i + j*j + k*k));
            dist_array->InsertNextTuple1(dist);
            rank_array->InsertNextTuple1(rank);
            }
          }
        }
      }

    icmin += lstep;
  }

  grid->SetPoints(points.GetPointer());
  grid->SetCells(VTK_HEXAHEDRON, hexs.GetPointer());
  grid->GetCellData()->AddArray(dist_array.GetPointer());
  grid->GetCellData()->AddArray(rank_array.GetPointer());
}

//-----------------------------------------------------------------------------
void PyramidGridGenerator(int rank, int wholeExtent[6], int extent[6],
                          vtkUnstructuredGrid *grid, int size)
{
  double nx = wholeExtent[1] - wholeExtent[0] + 1;
  double ny = wholeExtent[3] - wholeExtent[2] + 1;
  double nz = wholeExtent[5] - wholeExtent[4] + 1;

  int lnx = extent[1] - extent[0] + 1;
  int lny = extent[3] - extent[2] + 1;
  int lnz = extent[5] - extent[4] + 1;

  vtkNew<vtkPoints> points;
  points->Allocate((lnx + 1) * (lny + 1) * (lnz + 1));

  vtkNew<vtkCellArray> hexs;

  vtkNew<vtkFloatArray> dist_array;
  dist_array->SetName("Distance2Origin");
  dist_array->Allocate(lnx * lny * lnz);

  vtkNew<vtkUnsignedShortArray> rank_array;
  rank_array->SetName("Rank");
  rank_array->Allocate(lnx * lny * lnz);

  for (int i = extent[0]; i <= extent[1] + 1; i++)
    {
    double shift_z = nz * 0.5 * sin(((double)i + 1.) * PI / nx);

    for (int j = extent[2]; j <= extent[3] + 1; j++)
      {
      for (int k = extent[4]; k <= extent[5] + 1; k++)
        {
        double z = -(double)k + shift_z;
        points->InsertNextPoint(i, j, z);
        }
      }
    }

  for (int i = extent[0], ii = 0; i <= extent[1]; i++, ii++)
    {
    for (int j = extent[2], jj = 0; j <= extent[3]; j++, jj++)
      {
      for (int k = extent[4], kk = 0; k <= extent[5]; k++, kk++)
        {
        vtkIdType indice[8];
        indice[0] = (kk  ) + (jj  ) * (lnz + 1) + (ii  ) * (lnz + 1) * (lny + 1);
        indice[1] = (kk+1) + (jj  ) * (lnz + 1) + (ii  ) * (lnz + 1) * (lny + 1);
        indice[2] = (kk+1) + (jj+1) * (lnz + 1) + (ii  ) * (lnz + 1) * (lny + 1);
        indice[3] = (kk  ) + (jj+1) * (lnz + 1) + (ii  ) * (lnz + 1) * (lny + 1);
        indice[4] = (kk  ) + (jj  ) * (lnz + 1) + (ii+1) * (lnz + 1) * (lny + 1);
        indice[5] = (kk+1) + (jj  ) * (lnz + 1) + (ii+1) * (lnz + 1) * (lny + 1);
        indice[6] = (kk+1) + (jj+1) * (lnz + 1) + (ii+1) * (lnz + 1) * (lny + 1);
        indice[7] = (kk  ) + (jj+1) * (lnz + 1) + (ii+1) * (lnz + 1) * (lny + 1);

       double dist = sqrt(static_cast<double>(i*i + j*j + k*k));

        if ((i + j) <= size)
          {
          if ((k >= i) || (k >= j))
            {
            hexs->InsertNextCell(8, indice);
            dist_array->InsertNextTuple1(dist);
            rank_array->InsertNextTuple1(rank);
            }
          }
        else if (((i + j) > size) && (i < size) && (j < size))
          {
          if ((k >= (size- i)) || (k >= (size - j)))
            {
            hexs->InsertNextCell(8, indice);
            dist_array->InsertNextTuple1(dist);
            rank_array->InsertNextTuple1(rank);
            }
          }
        else
          {
          hexs->InsertNextCell(8, indice);
          dist_array->InsertNextTuple1(dist);
          rank_array->InsertNextTuple1(rank);
          }
        }
      }
    }

  grid->SetPoints(points.GetPointer());
  grid->SetCells(VTK_HEXAHEDRON, hexs.GetPointer());
  grid->GetCellData()->AddArray(dist_array.GetPointer());
  grid->GetCellData()->AddArray(rank_array.GetPointer());
}
