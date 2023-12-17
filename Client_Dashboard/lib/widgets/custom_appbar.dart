/*
  Project: CEMP Mobility Water Monitoring Station
  Author: Nguyen Trong Tin
  Email:tin.nguyen.31k17@hcmut.edu.vn
  Description: This code is part of the CEMP Mobility Water Monitoring Station project.
*/

import 'package:flutter/material.dart';

class CustomAppBar extends StatelessWidget implements PreferredSizeWidget {
  final String title;
  final bool showBackButton;

  const CustomAppBar({
    Key? key,
    required this.title,
    this.showBackButton = true,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return AppBar(
      leading: showBackButton ? InkWell(
        onTap: () => Navigator.pop(context),
        child: const Icon(
          Icons.arrow_back_sharp,
          color: Colors.black,
          size: 25,
        ),
      ) : null,
      title: Text(
        title,
        style: const TextStyle(
            fontSize: 18, color: Colors.black, fontWeight: FontWeight.w500),
      ),
      backgroundColor: Colors.transparent, // Make the AppBar background transparent
      elevation: 0, // Remove shadow
      centerTitle: true, // Center the title
    );
  }

  @override
  Size get preferredSize => Size.fromHeight(kToolbarHeight); // Standard AppBar height
}

