typedef unsigned long ulong;

class HashAlgorithm
{
// Fields
    ulong _hash;

// Constructor
	HashAlgorithm()
    {
		_hash = 0;
    }

// Properties
    ulong Hash { get { return _hash; } }

// Methods
    static ulong GetHash( string data )
    {
        return GetHash( data, 0 );
    }

    void AddString( string data )
    {
        _hash = GetHash( data, _hash );
    }

    void AddInt( int i )
    {
        _hash += ( _hash << 11 ) + (ulong)i;
    }

    void AddULong( ulong u )
    {
        _hash += ( _hash << 11 ) + u;
    }

    static private ulong GetHash( string data, ulong hash )
    {
        hash += ( hash << 13 ) + (ulong)data.Length;
        for ( int i = 0; i < data.Length; i++ )
            hash += ( hash << 17 ) + data[i];
        return hash;
    }

}
 
