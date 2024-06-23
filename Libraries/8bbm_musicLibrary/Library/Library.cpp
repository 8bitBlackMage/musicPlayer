//
// Created by Alice Exley on 22/06/2024.
//

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include "Library.h"


const juce::Identifier Library::idLibrary("Library");
const juce::Identifier Library::idArtistName("ArtistName");
const juce::Identifier Library::idAlbumName("AlbumName");
const juce::Identifier Library::idTrackTitle("TrackTitle");
const juce::Identifier Library::idTrackDuration("TrackDuration");
const juce::Identifier Library::idTrackFilePath("TrackPath");
const juce::Identifier Library::idTrackGenre("TrackGenre");
const juce::Identifier Library::idAlbumYear("albumYear");
const juce::Identifier Library::idTrackNumber("TrackNumber");


Library::Library()
{
    manager.registerBasicFormats();
    libraryTree = juce::ValueTree(idLibrary);
}

void Library::scanFolder(juce::File &rootFolder)
{

    juce::Array<juce::File> children = rootFolder.findChildFiles(juce::File::findFiles,
                                                            true,
                                                            manager.getWildcardForAllFormats());
    for(auto & child : children)
    {

        TagLib::FileRef file(child.getFullPathName().toRawUTF8());

        if(TagLib::Tag * properties = file.tag()) {
            juce::String artistName = juce::String::fromUTF8(properties->artist().toCString(true));
            juce::String albumName = juce::String::fromUTF8(properties->album().toCString(true));
            juce::String trackName = juce::String::fromUTF8(properties->title().toCString(true));

            if (artistName == "")
                artistName = "Unknown Artist";
            if (albumName == "")
                albumName = "Unknown Album";

            juce::String xmlSafeArtist = getXMLIdentifier(artistName,artist);
            juce::String xmlSafeAlbum = getXMLIdentifier(albumName,album);
            juce::String xmlSafeTrack = getXMLIdentifier(trackName,track);

            juce::int64 trackNumber = juce::int64(properties->track());
            juce::int64 albumYear = juce::int64(properties->year());
            juce::ValueTree artistTree = libraryTree.getOrCreateChildWithName(xmlSafeArtist, nullptr);
            if (!artistTree.hasProperty(idArtistName))
                artistTree.setProperty(idArtistName, artistName, nullptr);

            juce::ValueTree albumTree = artistTree.getOrCreateChildWithName(xmlSafeAlbum, nullptr);
            if (!albumTree.hasProperty(idAlbumName))
                albumTree.setProperty(idAlbumName, albumName, nullptr);
            if (!albumTree.hasProperty(idAlbumYear))
                albumTree.setProperty(idAlbumYear, albumYear, nullptr);

            juce::ValueTree trackTree(xmlSafeTrack);
            trackTree.setProperty(idTrackTitle, trackName, nullptr);
            trackTree.setProperty(idTrackFilePath, child.getFullPathName(), nullptr);
            trackTree.setProperty(idTrackGenre,
                                  juce::String::fromUTF8(properties->genre().toCString(true)),
                                  nullptr);
            
            trackTree.setProperty(idTrackNumber,trackNumber , nullptr);
            if (TagLib::AudioProperties *audioProperties = file.audioProperties())
            {
                trackTree.setProperty(idTrackDuration, audioProperties->lengthInSeconds(), nullptr);
            }


            albumTree.addChild(trackTree, -1, nullptr);
        }
    }
}

bool Library::writeToBinaryFile(juce::File &outputFile)
{
    try
    {
        juce::String libraryString = libraryTree.toXmlString();
        juce::FileOutputStream fileOutputStream(outputFile);
        juce::GZIPCompressorOutputStream compressor(fileOutputStream);
        compressor.write(libraryString.toRawUTF8(), libraryString.getNumBytesAsUTF8());
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        return false;
    }
    return true;
}

bool Library::readFromBinaryFile(juce::File &inputFile)
{
    auto iStream =  inputFile.createInputStream();
    if(iStream->openedOk())
    {
        juce::GZIPDecompressorInputStream stream(iStream.get(), false);
        juce::String libraryStream = stream.readEntireStreamAsString();
        libraryTree =  juce::ValueTree::fromXml(libraryStream);
        std::cout << libraryTree.toXmlString() <<std::endl;
        if(libraryTree.isValid())
            return true;
    }
    return false;
}

juce::ValueTree Library::getAlbumDataFromNameAndArtist(const juce::String &artistName, const juce::String &albumName)
{
    return juce::ValueTree();
}

juce::ValueTree Library::getAlbumDataFromName(const juce::String &albumName)
{
    if(libraryTree.isValid())
    {
        juce::String md5Identifier = getXMLIdentifier(albumName,album);

        for(int i =0; i < libraryTree.getNumChildren(); i ++)
        {
           juce::ValueTree artistTree =  libraryTree.getChild(i);
           juce::ValueTree albumTree = artistTree.getChildWithName(md5Identifier);
           if(albumTree.isValid())
           {
               sortValueTree(albumTree,idTrackNumber);
               return albumTree;
           }
        }
    }
    jassertfalse;
    return juce::ValueTree();
}

juce::ValueTree Library::getArtistDataFromName(const juce::String &artistName)
{
    if(libraryTree.isValid())
    {
       juce::String artistIdentifier = getXMLIdentifier(artistName, artist);

       juce::ValueTree artistTree = libraryTree.getChildWithName(artistIdentifier);
        sortValueTree(artistTree,idAlbumYear);

        return artistTree;
    }
    jassertfalse;
    return  juce::ValueTree();
}

void Library::sortValueTree(juce::ValueTree &tree,const juce::Identifier& sortKey)
{
    struct Sorter
    {
        explicit Sorter(const juce::Identifier& keyIn)
        : key(keyIn)
        {
        }
        int compareElements(const juce::ValueTree& first, const juce::ValueTree& second)
        {
            int firstTrack = first.getProperty(key);
            int secondTrack = second.getProperty(key);

            return firstTrack - secondTrack;
        }
        const juce::Identifier key;
    };
    Sorter sorter(sortKey);


    tree.sort(sorter, nullptr,false);
}

juce::String Library::getXMLIdentifier(const juce::String &value, Library::IdentifierType identifierType) {
    juce::String md5 = juce::MD5::fromUTF32(value).toHexString();
    switch(identifierType)
    {
        case album:
            return "album_" + md5;
        case artist:
            return "artist_"+ md5;
        case track:
            return "track_" + md5;
    }
}



