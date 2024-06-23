#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_cryptography/juce_cryptography.h>
#include <8bbm_musicLibrary/8bbm_musicLibrary.h>
//#include <fileref.h>
//#include <tag.h>
//juce::Identifier idLibrary("Library");
//juce::Identifier idTrack("Track");
//juce::Identifier idArtistName("TrackArtist");
//juce::Identifier idAlbumName("TrackAlbum");
//juce::Identifier idTrackTitle("TrackTitle");
//juce::Identifier idTrackDuration("TrackDuration");
//juce::Identifier idTrackFilePath("TrackFilePath");
//juce::Identifier idTrackGenre("TrackGenre");
//juce::Identifier idAlbumYear("TrackYear");
//juce::Identifier idTrackNumber("TrackNumber");
//
//
//void scanFolder(juce::File folder, juce::ValueTree libraryTree)
//{
//    juce::AudioFormatManager manager;
//    manager.registerBasicFormats();
//    juce::Array<juce::File> children =folder.findChildFiles(juce::File::findFiles,
//                                                            true,
//                          manager.getWildcardForAllFormats());
//
//    int i = 0;
//    for(auto & child : children)
//    {
//
//        TagLib::FileRef file(child.getFullPathName().toRawUTF8());
//
//        if(TagLib::Tag * properties = file.tag()) {
//            juce::String artist = juce::String::fromUTF8(properties->artist().toCString(true));
//            juce::String album = juce::String::fromUTF8(properties->album().toCString(true));
//            juce::String title = juce::String::fromUTF8(properties->title().toCString(true));
//
//            if (artist == "")
//                artist = "Unknown Artist";
//            if (album == "")
//                album = "Unknown Album";
//            juce::String xmlSafeArtist = juce::MD5::fromUTF32(artist).toHexString();
//            juce::String xmlSafeAlbum = juce::MD5::fromUTF32(album).toHexString();
//            juce::String xmlSafeTrack = juce::MD5::fromUTF32(title).toHexString();
//
//            juce::ValueTree artistTree = libraryTree.getOrCreateChildWithName(xmlSafeArtist, nullptr);
//            if (!artistTree.hasProperty(idArtistName))
//                artistTree.setProperty(idArtistName, artist, nullptr);
//
//            juce::ValueTree albumTree = artistTree.getOrCreateChildWithName(xmlSafeAlbum, nullptr);
//            if (!albumTree.hasProperty(idAlbumName))
//                albumTree.setProperty(idAlbumName, album, nullptr);
//
//
//            juce::ValueTree trackTree(xmlSafeTrack);
//            trackTree.setProperty(idTrackTitle, title, nullptr);
//            trackTree.setProperty(idTrackFilePath, child.getFullPathName(), nullptr);
//            trackTree.setProperty(idTrackGenre,
//                                  juce::String::fromUTF8(properties->genre().toCString(true)), nullptr);
//            trackTree.setProperty(idAlbumYear, juce::int64(properties->year()), nullptr);
//            trackTree.setProperty(idTrackNumber, juce::int64(properties->track()), nullptr);
//            if (TagLib::AudioProperties *audioProperties = file.audioProperties()) {
//                trackTree.setProperty(idTrackDuration, audioProperties->lengthInSeconds(), nullptr);
//            }
//
//
//            albumTree.addChild(trackTree, -1, nullptr);
//        }
//
//
//    }
//}
//
//

int main (int argc, char* argv[]) {
    juce::File rootDir = juce::File::getSpecialLocation(
            juce::File::SpecialLocationType::userMusicDirectory);

    Library library;
    library.scanFolder(rootDir);
    juce::File datFile = rootDir.getChildFile("music.dat");
    library.writeToBinaryFile(datFile);
    library.readFromBinaryFile(datFile);

    juce::ValueTree artistTree = library.getArtistDataFromName("Ghost Cassette");

    for(int i = 0; i < artistTree.getNumChildren();i++)
    {
        auto album = artistTree.getChild(i);
        std::cout << album.getProperty(Library::idAlbumName).toString()
        << " " << album.getProperty(Library::idAlbumYear).toString() << std::endl;
    }

}
