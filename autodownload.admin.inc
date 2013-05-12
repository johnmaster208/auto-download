<?php
/*
* autodownload_configure_pages()
* returns listing of all node/page download assignments with 'enabled' toggler checkbox
*/

//bool_yes
//quick and dirty for returning "Yes" or "No" for checkboxes.
function bool_yes($val){
    $b = ($val == 1)?"Yes":"No";
    return $b;
}
/*
*list_current_pages
*shows existing assignments for auto-downloads
*/
function autodownload_list_current_pages(){
    cache_clear_all();
    //grab the data
    $result = db_query("SELECT * FROM {autodownloads}");
    
    $headers = array(
        array('data' => t('Page')),  
        array('data' => t('Automatically Downloads...')),  
        array('data' => t('Enabled')),
        array('data' => t('Edit')),
        array('data' => t('Delete')),  
      );
      
    $caption =   'Listed below are the nodes which currently have automatic downloads assigned to them. 
      For this module version, only one (1) download per node may be assigned. If you create a new auto-download assignment 
      on a node which already has one, the pre-existing assignment will be lost. To edit an existing automatic 
      download configuration, click the "edit" link to the right of the node.';
    
    foreach($result as $row){
    $rows[] = array(
        array('data' => $row->nid),
        array('data' => $row->url),
        array('data' => bool_yes($row->enabled)),
        array('data' => l(t('Edit'), 'admin/config/user-interface/autodownload/edit' . '/' . $row->dlid)),
        array('data' => l(t('Delete'), 'admin/config/user-interface/autodownload/delete' . '/' . $row->dlid)),
      );
    }//end foreach
    if(empty($rows)){
     return "<h3>You haven't added any Auto-download Assignments yet.</h3><h1>Click the 'Create+' tab to get started!</h1>";
    }
    else{
     return theme('table', array('header' => $headers,'rows' => $rows, 'caption' => $caption));
    }
}

/*
*create_new_download
*allows admins to add a new assignment
*/
function autodownload_create_new_download_form($form, &$form_state){
    $form['new_download']['settings'] = array(
        '#type' => 'fieldset',
        '#title' => 'Create New Auto-download assignment',
        '#description' => 'Fill out the fields to create a new auto-download',
        '#collapsible' => TRUE,       
        );
    $form['new_download']['settings']['page'] = array(
        '#type' => 'textfield', 
        '#title' => t('Page or Node ID'),
        '#description' => t('Enter the name of the page or node in this field. It can be a node ID, such as node/123 or an alias, like products/phone-systems.'),
        '#length' => 150,
        '#element_validate' => array('autodownload_create_new_download_form_validate'),
        );
    $form['new_download']['settings']['url'] = array(
        '#type' => 'textarea',
        '#title' => t('File to be downloaded'),
        '#description' => t('Enter a link to the file which will be automatically downloaded.'),
        '#cols' => 50,
        '#rows' => 5,
        '#resizable' => TRUE,
    );
    $form['new_download']['settings']['enabled'] = array(
        '#title' => 'Enable Auto-Download',
        '#type' => 'checkbox',
        '#description' => t('Check this box to enable the auto-download immediately.'),  
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Create Auto-download'),
    );
    return $form;
}

//create new form submit
function autodownload_create_new_download_form_submit($form, &$form_state){
    //add the new entry as a record to {autodownloads}
    $table = 'autodownloads';
    $record = new stdClass();
    $record->nid = $form_state['values']['page'];
    $record->url = $form_state['values']['url'];
    $record->enabled = $form_state['values']['enabled'];
    $success = drupal_write_record($table, $record);
     
     if($success){
         $form_state['redirect'] = 'admin/config/user-interface/autodownload/list';
         drupal_set_message('The automatic download assignment for "'.$record->nid.'" was created.');
         cache_clear_all(); 
     }
    else{
        drupal_set_message("There was a problem adding an assignment. Please contact the admin for details.", "error");
    }
}
//create new form validate
function autodownload_create_new_download_form_validate($element, &$form_state){
    //add this later
}

/*
* autodownload_edit_node_form()
* 
*/
function autodownload_edit_node_form($form, &$form_state, $dlid){
    //query the {autodownloads} table for info related to our parameter
    $result = db_query("SELECT * FROM {autodownloads} WHERE {autodownloads}.dlid = :dlid", array(':dlid' => $dlid));
    $form['edit_download']['settings'] = array(
         '#type' => 'fieldset',
         '#title' => 'Edit this Auto-download assignment',
         '#description' => 'Fill out the fields to edit this auto-download',
         '#collapsible' => FALSE,       
         );
    foreach($result as $row){
     $form['edit_download']['settings']['page'] = array(
         '#type' => 'textfield',
         '#title' => t('Page or Node ID'),
         '#description' => t('Enter the name of the page or node in this field. It can be a node ID, such as node/123 or an alias, like products/phone-systems.'),
         '#length' => 150,
         '#default_value' => $row->nid,
         );
     $form['edit_download']['settings']['url'] = array(
         '#type' => 'textarea',
         '#title' => t('File to be downloaded'),
         '#description' => t('Enter a link to the file which will be automatically downloaded.'),
         '#cols' => 50,
         '#rows' => 5,
         '#resizable' => TRUE,
         '#default_value' => $row->url,
         );
     $form['edit_download']['settings']['enabled'] = array(
         '#title' => 'Enable Auto-Download',
         '#type' => 'checkbox',
         '#description' => t('Check this box to enable the auto-download.'),
         '#default_value' => $row->enabled,
         );
    $form['edit_download']['settings']['dlid'] = array(
         '#type' => 'hidden',
         '#value' => $row->dlid, 
     );
     }//end foreach
     $form['submit'] = array(
         '#type' => 'submit',
         '#value' => t('Save Configuration'),
         );
     return $form;
}


//edit form submit
function autodownload_edit_node_form_submit($form, &$form_state){
    $success = db_query("UPDATE {autodownloads} SET nid = :nid, url = :url, enabled = :enabled WHERE dlid = :dlid",
    array(':nid' => $form_state['values']['page'], ':url' => $form_state['values']['url'], ':enabled' => $form_state['values']['enabled'], ':dlid' => $form_state['values']['dlid'])
    );
     if($success){
         $form_state['redirect'] = 'admin/config/user-interface/autodownload/list';
         drupal_set_message('Successfully updated the download assignment.');
         cache_clear_all();
     }
    else{
        drupal_set_message('There was a problem adding an assignment. Please contact the admin for details.', 'error');
    }
}
//edit form validate
function autodownload_edit_node_form_validate($element, &$form_state){
    //add this later
}

function autodownload_delete_node_form($form, &$form_state, $dlid){
    //query the {autodownloads} table for info related to our parameter
    $result = db_query("SELECT * FROM {autodownloads} WHERE {autodownloads}.dlid = :dlid", array(':dlid' => $dlid));
    $form['delete_download']['settings'] = array(
         '#type' => 'fieldset',
         '#title' => 'Delete Auto-download assignment',
         '#collapsible' => FALSE,       
         );
    foreach($result as $row){
    $form['delete_download']['settings']['warning'] = array(
         '#markup' => "<strong>Are you sure you want to delete the auto-download assignment for '".$row->nid."'? This action cannot be undone.</strong>",
         );
     $form['delete_download']['settings']['dlid'] = array(
         '#type' => 'hidden',
         '#value' => $row->dlid, 
         );
     }//end foreach
     $form['submit'] = array(
         '#type' => 'submit',
         '#value' => t('Delete Auto-Download'),
         );
     return $form;
}

//delete form submit
function autodownload_delete_node_form_submit($form, &$form_state){
    $success = db_query("DELETE FROM {autodownloads} WHERE dlid = :dlid",array(':dlid' => $form_state['values']['dlid']));
     if($success){
         $form_state['redirect'] = 'admin/config/user-interface/autodownload/list';
         drupal_set_message("Successfully deleted auto-assignment.");
         cache_clear_all();
     }
    else{
        drupal_set_message('There was a problem adding an assignment. Please contact the admin for details.', 'error');
    }
}
//edit form validate
function autodownload_delete_node_form_validate($element, &$form_state){
    //add this later
}
/*
* autodownload_configure_advanced_form()
* additional settings for auto-downloads; timeOut, UI Dialogs and enhanced library integration
*/

function autodownload_configure_advanced_form($form, &$form_state){
    //Download method: can be simple, html5 or jQuery UI
    $form['advanced_settings']['javascript']['method'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#title' => t('Method Settings'),
      '#description' => 'Choose the method by which the auto-download initiates. The two (2) methods are as follows: 
       <ul>
          <li>Simple -- Uses a standard "window.open() function to an external URL</li> 
          <li>iFrame -- Adds a borderless iFrame to the document with "src" attribute set to the target URL</li>
          <li>HTML5 -- Instantiates an HTML5 hyperlink on the page with the "download" attribute set to the URL</li>
      </ul>',
    );
    $form['advanced_settings']['javascript']['method']['autodl_method'] = array(
        '#type' => 'select',
        '#title' => 'Enable jQuery UI Dialog popups for File Downloads',
        '#default_value' => variable_get('autodl_method','simple'),
        '#options' => array(
            'simple' => t('Simple'),
            'iframe' => t('iFrame'),
            'html5' => t('HTML5'),
        ),
    );    
    //jqueryUI settings: allows configuring of jQuery UI dialog window for download
    $form['advanced_settings']['javascript']['jqueryui'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#title' => t('jQuery Dialog Settings'),
      '#description' => t('Integration of jQuery UI Dialogs'),
    );
    $form['advanced_settings']['javascript']['jqueryui']['autodl_dialog1'] = array(
      '#type' => 'checkbox',
      '#default_value' => variable_get('autodl_dialog1'), 
      '#title' => t('Enable standard jQuery UI Dialog for autodownloads.'),
      '#description' => t('Enabling this option will allow you to use a simple jQuery UI dialog with the auto-download. The title and caption will be built for you.'),
    );
    //timeout settings: set to a second interval, breaks down to ms later...
    $form['advanced_settings']['javascript']['timeouts'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#title' => t('Timeout Settings'),
      '#description' => t('Timeout settings allow for a second delay for your download'),
    );
    $form['advanced_settings']['javascript']['timeouts']['autodl_timeout'] = array(
      '#type' => 'checkbox',
      '#default_value' => variable_get('autodl_timeout'), 
      '#title' => t('Enable setTimeout function for node pages.'),
      '#description' => t('Enabling this option will allow you to set a timer on all auto-downloads. The setTimeout function will default to 5 seconds (5000ms).'), 
    );
    $form['advanced_settings']['javascript']['timeouts']['autodl_timeout_seconds'] = array(
      '#type' => 'textfield',
      '#default_value' => variable_get('autodl_timeout_seconds',5),
      '#size' => 1, 
      '#title' => t('Timeout in Seconds.'),
      '#description' => t('Enter the value in seconds for timeouts. Default is set to 5 seconds.'), 
    );
    //submit the form
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save Configuration'),
    );
    return $form;
}
function autodownload_configure_advanced_form_submit($form, &$form_state){  
    //set variables for the form submissions
    variable_set('autodl_method', $form_state['values']['autodl_method']);
    if($form_state['values']['autodl_timeout'] == 1){
    variable_set('autodl_timeout',1);
    }
    variable_set('autodl_timeout_seconds', $form_state['values']['autodl_timeout_seconds']);
    if($form_state['values']['autodl_dialog1'] == 1){
    variable_set('autodl_dialog1',1);
    }
    else{
    variable_set('autodl_dialog1',0);
    }
}
?>