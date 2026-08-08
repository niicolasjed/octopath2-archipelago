#pragma once
// Table des quetes secondaires generee automatiquement - NE PAS editer a la main
// Usage : boucler sur subquest_table et lire MissionState[area][position]

struct SubquestEntry { int area; int position; int location_id; };

inline const SubquestEntry subquest_table[] = {
    { 0, 0, 6560300 }, // SS_SNW1_0100 : Le repaire des voleurs
    { 0, 1, 6560301 }, // SS_SNW1_0200 : Le bébé arrive !
    { 0, 2, 6560302 }, // SS_SNW2_0100 : Un cadeau pour mon fils
    { 0, 3, 6560303 }, // SS_SNW2_0200 : L'épée coincée dans la roche
    { 0, 4, 6560304 }, // SS_SNW3_0100 : Malaise au magasin
    { 0, 5, 6560305 }, // SS_SNW3_0200 : Sentiments tronqués
    { 0, 6, 6560306 }, // JNP_MEL_000 : Mélia, la suite
    { 0, 7, 6560307 }, // SS_TUT_009 : La sacoche du voyageur
    { 0, 8, 6560308 }, // SS_SEA2_0400 : Une bouteille à la mer
    { 0, 9, 6560309 }, // MARCHANT_EX02_0100 : MARCHANT_EX02_0100
    { 0, 10, 6560310 }, // MARCHANT_EX02_0200 : MARCHANT_EX02_0200
    { 0, 11, 6560311 }, // MARCHANT_EX02_0310 : MARCHANT_EX02_0310
    { 0, 12, 6560312 }, // MARCHANT_EX02_03A0 : MARCHANT_EX02_03A0
    { 0, 13, 6560313 }, // MARCHANT_EX02_03A1 : MARCHANT_EX02_03A1
    { 0, 14, 6560314 }, // MARCHANT_EX02_03B0 : MARCHANT_EX02_03B0
    { 0, 15, 6560315 }, // MARCHANT_EX02_03B1 : MARCHANT_EX02_03B1
    { 0, 16, 6560316 }, // MARCHANT_EX02_03C0 : MARCHANT_EX02_03C0
    { 0, 17, 6560317 }, // MARCHANT_EX02_03C1 : MARCHANT_EX02_03C1
    { 0, 18, 6560318 }, // MARCHANT_EX02_0400 : MARCHANT_EX02_0400
    { 0, 19, 6560319 }, // MARCHANT_EX02_0410 : MARCHANT_EX02_0410
    { 0, 20, 6560320 }, // MARCHANT_EX02_0500 : MARCHANT_EX02_0500
    { 5, 0, 6560800 }, // SS_ISD1_0100 : Une passerelle entre les peuples
    { 5, 1, 6560801 }, // SS_ISD1_0200 : Connaissances en cuisine
    { 5, 2, 6560802 }, // SS_ISD2_0100 : Le pêcheur qui préférait dormir
    { 5, 3, 6560803 }, // SS_ISD2_0200 : À la recherche de la perle
    { 5, 4, 6560804 }, // SS_ISD2_0300 : Comédien de théatre
    { 5, 5, 6560805 }, // SS_ISD3_0100 : Apprentissage de la langue humaine
    { 5, 6, 6560806 }, // JNP_MOR_000 : Sirlut, la suite
    { 5, 7, 6560807 }, // SS_TUT_004 : La sacoche du voyageur
    { 5, 8, 6560808 }, // SS_TUT_005 : La sacoche du voyageur
    { 5, 9, 6560809 }, // SS_Galdera : Porte vers un autre monde
    { 5, 10, 6560810 }, // MARCHANT_EX01_0100 : MARCHANT_EX01_0100
    { 5, 11, 6560811 }, // MARCHANT_EX01_0110 : MARCHANT_EX01_0110
    { 5, 12, 6560812 }, // MARCHANT_EX01_0200 : MARCHANT_EX01_0200
    { 5, 13, 6560813 }, // MARCHANT_EX01_0400 : MARCHANT_EX01_0400
    { 5, 14, 6560814 }, // MARCHANT_EX01_0410 : MARCHANT_EX01_0410
    { 5, 15, 6560815 }, // MARCHANT_EX01_0500 : MARCHANT_EX01_0500
    { 5, 16, 6560816 }, // MARCHANT_EX01_0600 : MARCHANT_EX01_0600
    { 5, 17, 6560817 }, // MARCHANT_EX01_0700 : MARCHANT_EX01_0700
    { 1, 0, 6560400 }, // SS_CTY1_0100 : L'habitué de la salle de jeux
    { 1, 1, 6560401 }, // SS_CTY1_0200 : Jeune nanti égoïste
    { 1, 2, 6560402 }, // SS_CTY1_0400 : Trop c'est trop !
    { 1, 3, 6560403 }, // SS_CTY1_0500 : Un menu étonnant
    { 1, 4, 6560404 }, // SS_CTY1_0700 : L'inventeur de génie
    { 1, 5, 6560405 }, // SS_CTY2_0100 : Catherine
    { 1, 6, 6560406 }, // SS_CTY2_0200 : Descendant d'une famille royale
    { 1, 7, 6560407 }, // SS_CTY2_0300 : La cloche de la tour
    { 1, 8, 6560408 }, // JNP_YJB_000 : Véronica, la suite
    { 1, 9, 6560409 }, // SS_TUT_003 : La sacoche du voyageur
    { 3, 0, 6560600 }, // SS_MNT1_0100 : La sécurité des pèlerins
    { 3, 1, 6560601 }, // SS_MNT1_0300 : Réparation du vitrail brisé
    { 3, 2, 6560602 }, // SS_MNT2_0100 : Chercheur en rade
    { 3, 3, 6560603 }, // SS_MNT2_0200 : Les trois grands mystères du monde
    { 3, 4, 6560604 }, // SS_MNT2_0300 : Le roi de l'arène
    { 3, 5, 6560605 }, // SS_MNT2_0400 : Au fin fond de l'enfer
    { 3, 6, 6560606 }, // SS_WLD2_0100 : Boîte mystérieuse
    { 3, 7, 6560607 }, // JNP_ALT_000 : Ort le chevalier, la suite
    { 3, 8, 6560608 }, // JNP_LYL_000 : Laïla, la suite
    { 3, 9, 6560609 }, // SS_TUT_001 : La sacoche du voyageur
    { 6, 0, 6560900 }, // SS_WLD1_0100 : La marchandise volée
    { 6, 1, 6560901 }, // SS_WLD1_0200 : Pour un beau sourire
    { 6, 2, 6560902 }, // SS_WLD1_0300 : Monstre de la mine
    { 6, 3, 6560903 }, // SS_WLD3_0200 : La fille disparue
    { 6, 4, 6560904 }, // SS_WLD3_0300 : Arme divine
    { 6, 5, 6560905 }, // SS_WLD3_0400 : Une vedette est née
    { 6, 6, 6560906 }, // JNP_APN_000 : Alpioné, la suite
    { 6, 7, 6560907 }, // SS_TUT_006 : La sacoche du voyageur
    { 2, 0, 6560500 }, // SS_SEA1_0100 : Les retrouvailles
    { 2, 1, 6560501 }, // SS_SEA1_0200 : L'objet perdu
    { 2, 2, 6560502 }, // SS_SEA1_0300 : Restauration du phare
    { 2, 3, 6560503 }, // SS_SEA2_0100 : Maigres vendanges
    { 2, 4, 6560504 }, // SS_SEA2_0200 : Voleur de poissons
    { 2, 5, 6560505 }, // SS_SEA2_0300 : Les souhaits d'une jeune fille
    { 2, 6, 6560506 }, // JNP_CLA_000 : Dame Clarissa, la suite
    { 2, 7, 6560507 }, // JNP_FAS_000 : Floyd et Thurston, la suite
    { 2, 8, 6560508 }, // SS_TUT_007 : La sacoche du voyageur
    { 4, 0, 6560700 }, // SS_TUT_002 : La sacoche du voyageur
    { 4, 1, 6560701 }, // SS_DST2_0100 : L'épéiste du temple
    { 4, 2, 6560702 }, // SS_DST2_0200 : Plans d'un pays disparu
    { 4, 3, 6560703 }, // SS_DST3_0100 : Le meilleur des deux
    { 4, 4, 6560704 }, // SS_DST3_0200 : Le trésor perdu de Koù
    { 4, 5, 6560705 }, // SS_DST3_0300 : Chevaux échappés
    { 4, 6, 6560706 }, // SS_DST3_0400 : Les épreuves de la Pagode
    { 4, 7, 6560707 }, // JNP_MIK_000 : Mikka, la suite
    { 4, 8, 6560708 }, // MARCHANT_EX03_0100 : MARCHANT_EX03_0100
    { 4, 9, 6560709 }, // MARCHANT_EX03_0110 : MARCHANT_EX03_0110
    { 4, 10, 6560710 }, // MARCHANT_EX03_0200 : MARCHANT_EX03_0200
    { 4, 11, 6560711 }, // MARCHANT_EX03_0300 : MARCHANT_EX03_0300
    { 4, 12, 6560712 }, // MARCHANT_EX03_0310 : MARCHANT_EX03_0310
    { 4, 13, 6560713 }, // MARCHANT_EX03_0400 : MARCHANT_EX03_0400
    { 4, 14, 6560714 }, // MARCHANT_EX03_0500 : MARCHANT_EX03_0500
    { 4, 15, 6560715 }, // MARCHANT_EX03_0600 : MARCHANT_EX03_0600
    { 7, 0, 6561000 }, // SS_FST1_0100 : Le retour du tissu Boisseaux
    { 7, 1, 6561001 }, // SS_FST1_0200 : Le noble qui avait l'alcool mauvais
    { 7, 2, 6561002 }, // SS_FST2_0100 : Un fruit inutile
    { 7, 3, 6561003 }, // SS_FST2_0200 : À travers les yeux d'un enfant
    { 7, 4, 6561004 }, // SS_FST3_0100 : La preuve de l'innocence
    { 7, 5, 6561005 }, // SS_FST3_0200 : Preuve de culpabilité
    { 7, 6, 6561006 }, // SS_FST3_0300 : Mariage forcé
    { 7, 7, 6561007 }, // SS_FST3_0400 : Preuve de justice
    { 7, 8, 6561008 }, // JNP_RAM_000 : Mira et le vieux gardien, la suite
    { 7, 9, 6561009 }, // JNP_SHI_000 : Misha, la suite
    { 7, 10, 6561010 }, // SS_TUT_008 : La sacoche du voyageur
    { 8, 0, 6561100 }, // SS_Galdera : Porte vers un autre monde
};
inline const int subquest_table_count = 104;
