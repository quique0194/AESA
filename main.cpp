#include <windows.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

typedef char distance_type;

template< class m_object_type >
class AESA
{
    typedef unsigned int uint;
    typedef distance_type (*distance_function)( const m_object_type &, const m_object_type & );

public:
    AESA( const vector< m_object_type > &_objs, distance_function _distance ): objs(_objs), distance(_distance)
    {
        create_matrix();
    }

    ~AESA()
    {
        delete_matrix();
    }

    vector< pair< m_object_type, distance_type > > query( const m_object_type &q, distance_type r )
    {
        vector< pair< m_object_type, distance_type > > ret;
        get_candidates( q, r, ret );
        return ret;
    }


private:

    vector< m_object_type > objs;
    distance_type** matrix;
    distance_function distance;

    void create_matrix()
    {
        // Reservamos espacio para la matriz

        uint n = objs.size();
        matrix = new distance_type*[n-1];
        for( uint i = 0; i < n; ++i )
        {
            matrix[i] = new distance_type[i];
        }

        // Calculamos las distancias y llenamos la matriz

        for( uint i = 0; i < n; ++i )
        {
            cout << "Construccion: " << i << "/" << n << endl;
            system("cls");
            for( uint j = 0; j < i; ++j )
                matrix[i][j] = distance( objs[i], objs[j] );
        }

    }

    void delete_matrix()
    {
        for( uint i = 0; i < objs.size(); ++i )
            delete [] matrix[i];
        delete [] matrix;
    }

    void get_candidates( const m_object_type &q, distance_type r, vector< pair< m_object_type, distance_type > > &candidates)
    {
        // Hacemos que todos los objetos sean candidatos

        uint n = objs.size();
        bool pos_candidates[n];
        for( uint i = 0; i < n; ++i )
            pos_candidates[i] = 1;

        // Recorremos la matriz y filtramos los candidatos

        for( int i = n-1; i >= 0; --i )
        {
            if( pos_candidates[i] )
            {
                distance_type rp = distance( objs[i], q );
                if( rp <= r )
                    candidates.push_back( pair< m_object_type, distance_type >( objs[i], rp ) );
                for( uint j = 0; j < i; ++j)
                {
                    if( rp - r > matrix[i][j] || matrix[i][j] > rp + r )
                    {
                        pos_candidates[j] = 0;
                    }
                }
            }
        }
    }

};











#include "l_distance.h"

#define TOLERANCIA 2
#define MAXPAL 10


template< class T >
struct sort_candidates
{
    bool operator()( const pair<T, distance_type> &p1, const pair<T, distance_type> &p2)
    {
        return p1.second < p2.second;
    }
};


int main()
{

    typedef unsigned int uint;

    vector<string> m_espace;

    // Levantamos las palabras en m_espace

    ifstream ifile("dict.txt");
    string buffer;
    while( !ifile.eof() )
    {
        ifile >> buffer;
        m_espace.push_back(buffer);
    }
    ifile.close();

    /*
        m_espace.push_back("hola");
        m_espace.push_back("chola");
        m_espace.push_back("bola");
        m_espace.push_back("joya");
    */

    // Creamos la estructura de datos
    // El tiempo de creacion es n^2, así que puede demorar varios minutos
    // En mi caso tardó 30 minutos

    AESA<string> aesa( m_espace, &l_distance );

    // Creamos la interfaz

    while(1)
    {
        string q;
        cout << "Ingrese una palabra erronea: ";
        cin >> q;

        vector< pair< string, distance_type > > ret = aesa.query( q, TOLERANCIA );

        if( !ret.empty() )
        {
            sort( ret.begin(), ret.end(), sort_candidates<string>() );

            if( ret[0].second == 0 )
            {
                cout << "La palabra es correcta!" << endl;
            }
            else
            {
                cout << "Quiza quizo decir: " << endl;
                for( uint i = 0; i < ret.size() && i < MAXPAL; ++i )
                {
                    cout << "Distancia: " << (uint)ret[i].second << "\tPalabra: " << ret[i].first << endl;
                }
            }
        }
        else
        {
            cout << "No hay sugerencias" << endl;
        }


        system("pause");
        system("cls");
    }

    return 0;
}
